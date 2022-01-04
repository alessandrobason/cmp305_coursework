#pragma once

#include <vector>
#include <string>

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags

#include "BaseMesh.h"
#include "TextureIdManager.h"
#include "types.h"

/* MMesh structure, almost identical to BaseMesh but:
 * - has a diffuseColor and textureId value that can be easily
 *   passed to a shader
 * - will cleanup its data in the deconstructor
 * - can't be copied, this is to avoid use after free and double-free segfaults
 * - can use a BaseMesh using moveFromMesh (like mmesh.moveFromMesh(new CubeMesh(...)))
 * - can also be used as a temporary container of a BaseMesh using clear at the end, e.g.:
 *    -- MMesh temp(cubeBaseMesh);
 *    -- shader->render(ctx, temp);
 *    -- temp.clear();
 *   clear will prevent MMesh from releasing the underlyling BaseMesh
 */
struct MMesh : public BaseMesh {
	using PubVertexType = VertexType;

	float4 diffuseColor = float4(0.f, 0.f, 0.f, 0.f);
	int textureId = 0;

	MMesh() = default;
	~MMesh();
	MMesh(MMesh &&other);
	MMesh(BaseMesh *other, float4 diffuseColor = float4(0.f, 0.f, 0.f, 1.f), int textureId = 0);

	// will move all the data from other to self
	void moveFromMesh(BaseMesh *other);
	// clears self data, can be safely deleted afterwards
	void clear();

	inline ID3D11Buffer *getVertexBuffer() {
		return vertexBuffer;
	}

	inline void setVertexBuffer(ID3D11Buffer *vbuf, int vcount) {
		vertexBuffer = vbuf;
		vertexCount = vcount;
	}

	inline ID3D11Buffer *getIndexBuffer() {
		return indexBuffer;
	}

	inline void setIndexBuffer(ID3D11Buffer *ibuf, int icount) {
		indexBuffer = ibuf;
		indexCount = icount;
	}

protected:
	void initBuffers(ID3D11Device *device) override;
};

// MModel is pretty much just a vector of MMeshes that can't be copied
// for the same reason a MMesh can't be copied
struct MModel {
	std::vector<MMesh> meshes;

	MModel() = default;
	MModel(MModel &&other);
};

/* MModelLoader can load almost any model using assimp, it will load the
 * material's diffuse color and texture, even when:
 * - it is embedded as a png/jpg file
 * - it is embedded as bgra data
 * - it is a string of a png/jpg file
 */
class MModelLoader {
public:
	void init(ID3D11Device *dev, TextureIdManager *textureMgr);
	// Returns a pointer to a MModel structure, the pointer is allocated
	// with new and should be deleted
	MModel *load(const std::string &file);

private:
	void processNode(const aiNode *node);
	void processMesh(const aiMesh *mesh);
	int processTexture(const aiMaterial *mat);

	using VertexType = MMesh::PubVertexType;

	ID3D11Device *device = nullptr;
	TextureIdManager *tmanager = nullptr;
	const aiScene *scene = nullptr;

	MModel model;
	std::vector<VertexType> vertices;
	std::vector<ulong> indices;
};
