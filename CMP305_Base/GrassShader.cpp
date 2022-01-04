#include "GrassShader.h"

#include "utility.h"

GrassShader::GrassShader(Device *device, HWND hwnd)
	: DefaultShader(device, hwnd) {
	initShader(L"grass_vs.cso", L"grass_ps.cso");

	loadHullShader(L"grass_hs.cso");
	loadDomainShader(L"grass_ds.cso");
	loadGeometryShader(L"grass_gs.cso");

	addDynamicBuffer<HullBuffer>(&hullBuf);
	addDynamicBuffer<GrassBuffer>(&grassBuf);
}

GrassShader::~GrassShader() {
	SAFE_RELEASE(hullBuf);
	SAFE_RELEASE(grassBuf);
}

void GrassShader::setShaderParameters(
	DeviceContext *ctx,
	const mat4 &world,
	const mat4 &view,
	const mat4 &proj,
	TextureType *heightmap,
	TextureType *normalmap,
	TextureType *texture,
	Light &light,
	float3 cameraPos,
	float slopeThreshold,
	float cutoffDistance,
	float maxDist,
	float maxFactor
) {
	// == MATRIX BUFFER =========================
	// Transpose the matrices to prepare them for the shader.
	mat4 tworld = XMMatrixTranspose(world);
	mat4 tview = XMMatrixTranspose(view);
	mat4 tproj = XMMatrixTranspose(proj);

	auto matrixPtr = MappedBuffer<MatrixBufferType>(ctx, matrixBuffer, GS, 0);
	matrixPtr->world = tworld;
	matrixPtr->view = tview;
	matrixPtr->projection = tproj;

	// == HULL BUFFER ===========================
	auto hull = MappedBuffer<HullBuffer>(ctx, hullBuf, HS, 0);
	hull->cameraPos = cameraPos;
	hull->cutoffDistance = cutoffDistance;
	hull->maxDist = maxDist;
	hull->maxFactor = maxFactor;

	// == GRASS BUFFER ==========================
	auto grass = MappedBuffer<GrassBuffer>(ctx, grassBuf, GS, 1);
	grass->cameraPos = cameraPos;
	grass->slopeThreshold = slopeThreshold;

	// == LIGHT BUFFER ==========================
	auto lightPtr = MappedBuffer<LightBufferType>(ctx, lightBuffer, PS, 0);
	lightPtr->diffuse = light.getDiffuseColour();
	lightPtr->direction = light.getDirection();
	lightPtr->padding = 0.f;

	// == SHADER RESOURCES ======================
	ctx->VSSetShaderResources(0, 1, &heightmap);

	ctx->GSSetShaderResources(0, 1, &normalmap);
	ctx->GSSetSamplers(0, 1, &sampleState);

	ctx->PSSetShaderResources(0, 1, &texture);
	ctx->PSSetSamplers(0, 1, &sampleState);
}

void GrassShader::render(DeviceContext *ctx, MMesh &mesh) {
	DefaultShader::render(ctx, mesh, D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
}

GrassPatternShader::GrassPatternShader(Device *device, HWND hwnd)
	: PixelOnlyShader(device, hwnd) {
	initShader(L"grass_pattern_ps.cso");
	addDynamicBuffer<GrassBuffer>(&grassBuf);
}

GrassPatternShader::~GrassPatternShader() {
	SAFE_RELEASE(grassBuf);
}

void GrassPatternShader::setData(
	DeviceContext *ctx, 
	float2 resolution,
	float2 offset,
	float scale, 
	float exponent, 
	float threshold
) {
	auto buf = MappedBuffer<GrassBuffer>(ctx, grassBuf, PS, 0);
	buf->resolution = resolution;
	buf->offset = offset;
	buf->scale = scale;
	buf->exponent = exponent;
	buf->threshold = threshold;
}

GrassCompute::GrassCompute(Device *device, HWND hwnd)
	: DefaultShader(device, hwnd) {
	loadComputeShader(L"grass_cs.cso");
}
