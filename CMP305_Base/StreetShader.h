#pragma once

#include "DefaultShader.h"

class StreetShader : public DefaultShader {
public:
	StreetShader(Device *device, HWND hwnd);
	~StreetShader();

	void setShaderParameters(DeviceContext *ctx, const mat4 &world, const mat4 &view, const mat4 &proj);

protected:
	void initShader(const wchar_t *vs, const wchar_t *ps);

};