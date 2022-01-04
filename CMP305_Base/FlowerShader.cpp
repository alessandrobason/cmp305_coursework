#include "FlowerShader.h"

#include "utility.h"

FlowerGenShader::FlowerGenShader(Device *device, HWND hwnd) 
	: PixelOnlyShader(device, hwnd) {
	initShader(L"flower_gen_ps.cso");
	addDynamicBuffer<BufferType>(&buffer);
}

FlowerGenShader::~FlowerGenShader() {
	SAFE_RELEASE(buffer);
}

void FlowerGenShader::setShaderParameters(
	DeviceContext *ctx, 
	const mat4 &world, 
	const mat4 &view, 
	const mat4 &proj, 
	float2 resolution, 
	float scale, 
	const float4 &seed
) {
	PixelOnlyShader::setShaderParameters(ctx, world, view, proj);

	auto buf = MappedBuffer<BufferType>(ctx, buffer, PS, 0);
	buf->resolution = resolution;
	buf->scale = scale;
	buf->seed = seed;
	buf->padding = 0.f;
}

FlowerShader::FlowerShader(Device *device, HWND hwnd)
	: DefaultShader(device, hwnd) {
	initShader(L"grass_vs.cso", L"grass_ps.cso");

	loadHullShader(L"grass_hs.cso");
	loadDomainShader(L"grass_ds.cso");
	loadGeometryShader(L"flower_gs.cso");

	addDynamicBuffer<HullBuffer>(&hullBuf);
	addDynamicBuffer<GrassBuffer>(&grassBuf);
}

FlowerShader::~FlowerShader() {
	SAFE_RELEASE(hullBuf);
	SAFE_RELEASE(grassBuf);
}

void FlowerShader::setShaderParameters(
	DeviceContext *ctx,
	const mat4 &world,
	const mat4 &view,
	const mat4 &proj,
	TextureType *heightmap,
	TextureType *normalmap,
	TextureType *flowermap,
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
	ctx->GSSetShaderResources(1, 1, &flowermap);
	ctx->GSSetSamplers(0, 1, &sampleState);

	ctx->PSSetShaderResources(0, 1, &texture);
	ctx->PSSetSamplers(0, 1, &sampleState);
}

void FlowerShader::render(DeviceContext *ctx, MMesh &mesh) {
	DefaultShader::render(ctx, mesh, D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
}
