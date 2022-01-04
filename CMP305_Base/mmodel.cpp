#include "mmodel.h"

#include "tracelog.h"
#include "utility.h"

#include <assimp/version.h>

MMesh::MMesh(MMesh &&other) {
	diffuseColor = other.diffuseColor;
	textureId = other.textureId;
	indexBuffer = other.indexBuffer;
	indexCount = other.indexCount;
	vertexBuffer = other.vertexBuffer;
	vertexCount = other.vertexCount;

	other.indexBuffer = other.vertexBuffer = nullptr;
}

MMesh::MMesh(BaseMesh *other, float4 diffuseColor, int textureId)
	: diffuseColor(diffuseColor), textureId(textureId) {
	// temporarily copy, probably just for a single draw
	indexBuffer = other->getIndexBuffer();
	vertexBuffer = other->getVertexBuffer();
	indexCount = other->getIndexCount();
	vertexCount = other->getVertexCount();
	vertexSize = other->getVertexSize();
}

void MMesh::moveFromMesh(BaseMesh *other) {
	indexBuffer = other->getIndexBuffer();
	vertexBuffer = other->getVertexBuffer();
	indexCount = other->getIndexCount();
	vertexCount = other->getVertexCount();
	vertexSize = other->getVertexSize();

	other->setIndexBuffer(0);
	other->setVertexBuffer(0);
	other->setIndexCount(0);
	other->setVertexCount(0);
}

void MMesh::clear() {
	indexBuffer = nullptr;
	vertexBuffer = nullptr;
	indexCount = 0;
	vertexCount = 0;
	diffuseColor = float4(0.f, 0.f, 0.f, 0.f);
	textureId = 0;
}

MMesh::~MMesh() {
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);
	clear();
}

void MMesh::initBuffers(ID3D11Device *device) {

}

MModel::MModel(MModel &&other) {
	meshes = std::move(other.meshes);
}

void MModelLoader::init(ID3D11Device *dev, TextureIdManager *textureMgr) {
	device = dev;
	tmanager = textureMgr;
}

MModel *MModelLoader::load(const std::string &file) {
	bool result = false;
	MModel *resultModel = nullptr;
	Assimp::Importer importer;

	scene = importer.ReadFile(
		file,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_MakeLeftHanded |
		aiProcess_FlipUVs
	);

	if (!scene) {
		err("Couldn't load %s: %s", file.c_str(), importer.GetErrorString());
		goto error;
	}

	processNode(scene->mRootNode);

	resultModel = new MModel(std::move(model));

error:
	return resultModel;
}

void MModelLoader::processNode(const aiNode *node) {
	for (uint i = 0; i < node->mNumMeshes; ++i) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh);
	}

	for (uint i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i]);
	}
}

void MModelLoader::processMesh(const aiMesh *in_mesh) {
	MMesh out_mesh;

	aiMaterial *mat = scene->mMaterials[in_mesh->mMaterialIndex];

	// Get diffuse color
	aiColor3D color(0.f, 0.f, 0.f);
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	out_mesh.diffuseColor = float4(color.r, color.g, color.b, 1.f);

	// Get texture, if it exists
	int texCount = mat->GetTextureCount(aiTextureType_DIFFUSE);
	if (texCount > 0) {
		out_mesh.textureId = processTexture(mat);
	}

	vertices.reserve(in_mesh->mNumVertices);
	// because we triangulate the texture, all faces are 3 sides
	indices.reserve((size_t)in_mesh->mNumFaces * 3);

	for (uint i = 0; i < in_mesh->mNumVertices; ++i) {
		float3 vert{
			in_mesh->mVertices[i].x,
			in_mesh->mVertices[i].y,
			in_mesh->mVertices[i].z,
		};
		float2 text = float2(0.f, 0.f);
		float3 norm = float3(0.f, 0.f, 0.f);

		if (in_mesh->HasTextureCoords(0)) {
			text.x = in_mesh->mTextureCoords[0][i].x;
			text.y = in_mesh->mTextureCoords[0][i].y;
		}

		if (in_mesh->HasNormals()) {
			norm.x = in_mesh->mNormals[i].x;
			norm.y = in_mesh->mNormals[i].y;
			norm.z = in_mesh->mNormals[i].z;
		}

		vertices.emplace_back(vert, text, norm);
	}

	for (uint i = 0; i < in_mesh->mNumFaces; ++i) {
		aiFace &face = in_mesh->mFaces[i];

		for (uint j = 0; j < face.mNumIndices; ++j) {
			indices.emplace_back(face.mIndices[j]);
		}
	}

	// Load buffers

	D3D11_BUFFER_DESC vbufDesc{}, ibufDesc{};
	D3D11_SUBRESOURCE_DATA vData{}, iData{};
	ID3D11Buffer *vbuf, *ibuf;
	// Set up the description of the static vertex buffer
	vbufDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufDesc.ByteWidth = uint(sizeof(VertexType) * vertices.size());
	vbufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// Give the subresource structure a pointer to the vertex data
	vData.pSysMem = vertices.data();
	// Create the vertex buffer
	device->CreateBuffer(&vbufDesc, &vData, &vbuf);

	// Set up the description of the static index buffer
	ibufDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufDesc.ByteWidth = uint(sizeof(ulong) * indices.size());
	ibufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// Give the subresource structure a pointer to the index data
	iData.pSysMem = indices.data();
	// Create the index buffer
	device->CreateBuffer(&ibufDesc, &iData, &ibuf);

	// Pass it to the current material
	out_mesh.setVertexBuffer(vbuf, (int)vertices.size());
	out_mesh.setIndexBuffer(ibuf, (int)indices.size());

	// push the material to the model
	model.meshes.emplace_back(std::move(out_mesh));

	// we don't need this data, clear it for the next material
	vertices.clear();
	indices.clear();
}

int MModelLoader::processTexture(const aiMaterial *mat) {
	// In Assimp textures can be saved in one of 3 ways:
	// - as a string
	// - as a blob of bgra pixel data
	// - as a blob of file data
	// In the first case we can simply load the image normally.
	// GetEmbeddedTexture returns null when the texture is NOT embedded.

	aiString texPath{};
	mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texPath);
	if (auto texture = scene->GetEmbeddedTexture(texPath.C_Str())) {
		// read texture from memory
		// if mHeight == 0, pcData is just a pointer to a file-like memory, e.g. a png/jpg file
		if (texture->mHeight == 0) {
			return tmanager->loadTexture(texture->pcData, texture->mWidth);
		}
		else {
			return tmanager->loadTexture((PixelData *)texture->pcData, texture->mWidth, texture->mHeight);
		}
	}
	else {
		// regular file, load it normally
		return tmanager->loadTexture(texPath.C_Str());
	}
	// unreachable
	return 0;
}
