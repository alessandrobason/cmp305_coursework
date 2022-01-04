#include "TextureIdManager.h"

#include <d3d11.h>

#include "DTK\include\DDSTextureLoader.h"
#include "DTK\include\WICTextureLoader.h"

#include "tracelog.h"
#include "utility.h"

void TextureIdManager::init(Device *ldevice, DeviceContext *lctx) {
	device = ldevice;
	ctx = lctx;
	addDefaultTexture();
}

TextureIdManager::~TextureIdManager() {
	for (ID3D11Texture2D *ptex : pTextures) {
		SAFE_RELEASE(ptex);
	}

	for (TextureType *tex : textures) {
		SAFE_RELEASE(tex);
	}
}

int TextureIdManager::loadTexture(const std::string &filename) {
	return loadTextureAt(filename, (int)textures.size());
}

int TextureIdManager::loadTexture(void *data, uint size) {
	TextureType *texture = nullptr;
	int index = -1;

	HRESULT result = DirectX::CreateWICTextureFromMemory(
		device,
		(uint8_t *)data, size,
		NULL, &texture
	);

	if (FAILED(result)) {
		err("Couldn't load texture, data: <%p>, size: <%u>", data, size);
	}
	else {
		index = (int)textures.size();
		textures.emplace_back(texture);
	}

	return index;
}

int TextureIdManager::loadTexture(void *data, uint width, uint height, int format) {
	int index = -1;
	
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	switch (format) {
	case RGBA: desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
	case BGRA: desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; break;
	default: assert(false);
	}

	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = data;
	initialData.SysMemPitch = sizeof(uint) * width;
	initialData.SysMemSlicePitch = 0;

	ID3D11Texture2D *texture2d = nullptr;

	HRESULT result = device->CreateTexture2D(&desc, &initialData, &texture2d);
	pTextures.emplace_back(texture2d);

	if (SUCCEEDED(result)) {
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = desc.MipLevels;

		TextureType *texture = nullptr;
		result = device->CreateShaderResourceView(texture2d, &SRVDesc, &texture);
		index = (int)textures.size();
		textures.emplace_back(texture);
	}
	else {
		err("failed to create texture, pixel data: <%p>, width: <%u>, height: <%u>, error: %d", data, width, height, result);
	}

	return index;
}

int TextureIdManager::loadTextureAt(const std::string &filename, int id) {
	int index = -1;

	std::string extension;
	wchar_t *wFilename = nullptr;
	TextureType *texture = nullptr;
	HRESULT result;

	if (!fileExists(filename.c_str())) {
		err("couldn't find file \"%s\"", filename.c_str());
		goto error;
	}

	auto idx = filename.rfind('.');

	if (idx != std::string::npos) {
		extension = filename.substr(idx + 1);
	}

	wFilename = wstrFromStr(filename.c_str(), filename.size());
	if (!wFilename) goto error;

	if (extension == "dds") {
		result = DirectX::CreateDDSTextureFromFile(
			device, ctx,
			wFilename, NULL,
			&texture, 0
		);
	}
	else {
		result = DirectX::CreateWICTextureFromFile(
			device, ctx,
			wFilename, NULL,
			&texture, 0
		);
	}

	delete[] wFilename;

	if (FAILED(result)) {
		err("Couldn't load texture %s -> %d", filename.c_str(), result);
		goto error;
	}
	else {
		index = id;
		if (index == textures.size()) {
			textures.emplace_back(texture);
		}
		else {
			textures[index] = texture;
		}
	}

error:
	return index;
}

bool TextureIdManager::reloadTexture(const std::string &filename, int id) {
	SAFE_RELEASE(textures[id]);
	int result = loadTextureAt(filename, id);
	return result == id;
}

TextureType *TextureIdManager::getTexture(int id) {
	// if the index is out of range, return the default texture
	if (id < 0 || id >= textures.size()) return textures[0];
	return textures[id];
}

TextureType *TextureIdManager::operator[](int id) {
	return getTexture(id);
}

void TextureIdManager::addDefaultTexture() {
	static constexpr uint32_t s_pixel = 0xffffffff;
	loadTexture((void *)&s_pixel, 1, 1);
}
