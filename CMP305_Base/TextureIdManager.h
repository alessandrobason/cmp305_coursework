#pragma once

#include <d3d11.h>
#include <vector>
#include <string>

#include "types.h"

#pragma pack(push, 1)
struct PixelData {
	uchar b, g, r, a;
};
#pragma pack(pop)

enum {
	RGBA, BGRA
};

/* The TextureIdManager class manages textures, the main difference
 * compared to the default TextureManager is that it uses a vector
 * underneath instead of a map, and the textures are identified by
 * indices. This makes it much more memory efficient to save the
 * texture id in meshes themselves.
 * When calling getTexture, if an out-of-range index is passed,
 * the default texture (a white texture) is returned.
 * Also, when checking if a file exists it uses a lower-level
 * win32 function instead of trying to open the file with ifstream.
 */
class TextureIdManager {
public:
	TextureIdManager() = default;
	~TextureIdManager();

	void init(Device *device, DeviceContext *ctx);

	int loadTexture(const std::string &filename);
	int loadTexture(void *data, uint size);
	int loadTexture(void *data, uint width, uint height, int format = RGBA);
	int loadTextureAt(const std::string &filename, int id);
	bool reloadTexture(const std::string &filename, int id);
	TextureType *getTexture(int id);
	TextureType *operator[](int id);

	// disable copy
	TextureIdManager(const TextureIdManager &other) = delete;
	TextureIdManager &operator=(TextureIdManager &other) = delete;

private:
	void addDefaultTexture();

	Device *device = nullptr;
	DeviceContext *ctx = nullptr;

	std::vector<TextureType *> textures;
	std::vector<ID3D11Texture2D *> pTextures;
};