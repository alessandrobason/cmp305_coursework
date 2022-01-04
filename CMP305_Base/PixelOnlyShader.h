#pragma once

#include "DefaultShader.h"

class PixelOnlyShader : public DefaultShader {
public:
	PixelOnlyShader(Device *device, HWND hwnd);
	void setShaderParameters(DeviceContext *ctx, const mat4 &world, const mat4 &view, const mat4 &proj);

protected:
	void initShader(const wchar_t *ps);
};