#pragma once

#include "PixelOnlyShader.h"

class CopyShader : public PixelOnlyShader {
public:
	CopyShader(Device *device, HWND hwnd);
	void setShaderParameters(DeviceContext *ctx, const mat4 &world, const mat4 &view, const mat4 &proj, TextureType *input);
};