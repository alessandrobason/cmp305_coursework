#include "StreetShader.h"

#include "utility.h"

StreetShader::StreetShader(Device *device, HWND hwnd) 
	: DefaultShader(device, hwnd) {
	initShader(L"street_vs.cso", L"street_ps.cso");
}

StreetShader::~StreetShader() {
}

void StreetShader::setShaderParameters(
	DeviceContext *ctx, 
	const mat4 &world, 
	const mat4 &view, 
	const mat4 &proj
) {
	// == MATRIX BUFFER =========================
	// Transpose the matrices to prepare them for the shader.
	mat4 tworld = XMMatrixTranspose(world);
	mat4 tview = XMMatrixTranspose(view);
	mat4 tproj = XMMatrixTranspose(proj);

	auto matrixPtr = MappedBuffer<MatrixBufferType>(ctx, matrixBuffer, VS, 0);
	matrixPtr->world = tworld;
	matrixPtr->view = tview;
	matrixPtr->projection = tproj;
}

void StreetShader::initShader(const wchar_t *vs, const wchar_t *ps) {
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	loadVertexShader(vs, polygonLayout, sizeof(polygonLayout) / sizeof(polygonLayout[0]));
	loadPixelShader(ps);

	addDynamicBuffer<MatrixBufferType>(&matrixBuffer);
}
