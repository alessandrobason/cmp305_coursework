#include "Erosion.h"

#include "utility.h"
#include "MathUtils.h"

// == BASE EROSION SHADER ========================================================

BaseErosionShader::BaseErosionShader(Device *device, HWND hwnd, const wchar_t *shader)
	: PixelOnlyShader(device, hwnd) {
	if (shader) initShader(shader);
	//addDiffuseSampler();
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);
}

BaseErosionShader::~BaseErosionShader() {
	SAFE_RELEASE(buffer);
}

void BaseErosionShader::setShaderParameters(
	DeviceContext *ctx,
	const mat4 &world,
	const mat4 &view,
	const mat4 &proj,
	TextureType *textures[3]
) {
	PixelOnlyShader::setShaderParameters(ctx, world, view, proj);
	ctx->PSSetShaderResources(0, 3, textures);
	ctx->PSSetSamplers(0, 1, &sampleState);
}

// == RAIN SHADER ================================================================

RainShader::RainShader(Device *device, HWND hwnd)
	: BaseErosionShader(device, hwnd) {
	initShader(L"rain_ps.cso");
	addDynamicBuffer<BufferType>(&buffer);
}

void RainShader::setData(
	DeviceContext *ctx, 
	float2 cellSize, 
	float prob, 
	float amount, 
	float2 seed
) {
	auto buf = MappedBuffer<BufferType>(ctx, buffer, PS, 0);
	buf->cellSize = cellSize;
	buf->rainProbability = prob;
	buf->rainAmount = amount;
	buf->seed = seed;
}

// == FLOW SHADER ================================================================

FlowShader::FlowShader(Device *device, HWND hwnd)
	: BaseErosionShader(device, hwnd) {
	initShader(L"flow_ps.cso");
	addDynamicBuffer<BufferType>(&buffer);
}	

void FlowShader::setData(
	DeviceContext *ctx, 
	float2 cellSize, 
	float gravity, 
	float area, 
	float len
) {
	auto buf = MappedBuffer<BufferType>(ctx, buffer, PS, 0);
	buf->cellSize = cellSize;
	buf->gravity = gravity;
	buf->pipeArea = area;
	buf->pipeLen = len;
}

// == WATER SHADER ===============================================================

WaterShader::WaterShader(Device *device, HWND hwnd)
	: BaseErosionShader(device, hwnd) {
	initShader(L"water_ps.cso");
	addDynamicBuffer<BufferType>(&buffer);
}

void WaterShader::setData(DeviceContext *ctx, float2 cellSize) {
	auto buf = MappedBuffer<BufferType>(ctx, buffer, PS, 0);
	buf->cellSize = cellSize;
}

// == VELOCITY SHADER ============================================================

VelocityShader::VelocityShader(Device *device, HWND hwnd)
	: BaseErosionShader(device, hwnd) {
	initShader(L"velocity_ps.cso");
	addDynamicBuffer<BufferType>(&buffer);
}

void VelocityShader::setData(DeviceContext *ctx, float2 cellSize) {
	auto buf = MappedBuffer<BufferType>(ctx, buffer, PS, 0);
	buf->cellSize = cellSize;
}

// == EROSION SHADER =============================================================

ErosionShader::ErosionShader(Device *device, HWND hwnd)
	: BaseErosionShader(device, hwnd) {
	initShader(L"erosion_ps.cso");
	addDynamicBuffer<BufferType>(&buffer);
}

void ErosionShader::setData(
	DeviceContext *ctx, 
	TextureType *normalmap,
	float2 cellSize,
	float capacity,
	float dissolving,
	float deposition,
	float evaporation
) {
	auto buf = MappedBuffer<BufferType>(ctx, buffer, PS, 0);
	buf->cellSize = cellSize;
	buf->sedimentCapacity = capacity;
	buf->dissolvingConst = dissolving;
	buf->depositionConst = deposition;
	buf->evaporationConst = evaporation;
	
	ctx->PSSetShaderResources(3, 1, &normalmap);
}

// == TRANSPORT SHADER ===========================================================

TransportShader::TransportShader(Device *device, HWND hwnd)
	: BaseErosionShader(device, hwnd) {
	initShader(L"transport_ps.cso");
	addDynamicBuffer<BufferType>(&buffer);
}

void TransportShader::setData(DeviceContext *ctx, float2 cellSize) {
	auto buf = MappedBuffer<BufferType>(ctx, buffer, PS, 0);
	buf->cellSize = cellSize;
}

// == EROSION ====================================================================

void Erosion::init(
	Device *device, 
	DeviceContext *ctx, 
	HWND hwnd, 
	int mapSize, 
	int screenWidth, 
	int screenHeight
) {
	heightMap  		= new RenderTexture(device, mapSize, mapSize, 0.1f, 100.f);
	fluxMap    		= new RenderTexture(device, mapSize, mapSize, 0.1f, 100.f);
	velocityMap		= new RenderTexture(device, mapSize, mapSize, 0.1f, 100.f);
	nextHeightMap	= new RenderTexture(device, mapSize, mapSize, 0.1f, 100.f);
	nextFluxMap		= new RenderTexture(device, mapSize, mapSize, 0.1f, 100.f);

	copyShader      = new CopyShader(device, hwnd);
	rainShader      = new RainShader(device, hwnd);
	flowShader      = new FlowShader(device, hwnd);
	waterShader     = new WaterShader(device, hwnd);
	velocityShader  = new VelocityShader(device, hwnd);
	erosionShader   = new ErosionShader(device, hwnd);
	transportShader = new TransportShader(device, hwnd);

	orthoMesh.moveFromMesh(new OrthoMesh(device, ctx, screenWidth, screenHeight));
}

void Erosion::update(
	DeviceContext *ctx, 
	const mat4 &world, 
	const mat4 &view, 
	const mat4 &proj, 
	TextureType *normalmap
) {
	if (!isSimulating) return;

	// clear all the render textures
	nextHeightMap->clearRenderTarget(ctx, 0, 0, 0, 1);
	nextFluxMap->clearRenderTarget(ctx, 0, 0, 0, 1);

	TextureType *textures[3]{
		heightMap->getShaderResourceView(),
		fluxMap->getShaderResourceView(),
		nullptr
	};

	float2 seed{ randRange<f32>(-100.f, 100.f), randRange<f32>(-100.f, 100.f) };

	// == 1st pass - rain ========================================================================

	if (isRaining) {
		// Use rain shader to update heightmap
		nextHeightMap->setRenderTarget(ctx);
		rainShader->setData(ctx, cellSize, rainProbability, rainAmount, seed);
		rainShader->setShaderParameters(ctx, world, view, proj, textures);
		rainShader->render(ctx, orthoMesh);
	}
	else {
		// Copy heightmap to nextHeightMap for the next few passes
		nextHeightMap->setRenderTarget(ctx);
		copyShader->setShaderParameters(ctx, world, view, proj, heightMap->getShaderResourceView());
		copyShader->render(ctx, orthoMesh);
	}

	textures[0] = nextHeightMap->getShaderResourceView();

	// == 2nd pass - flux velocity calculation ===================================================
	
	nextFluxMap->setRenderTarget(ctx);
	flowShader->setShaderParameters(ctx, world, view, proj, textures);
	flowShader->setData(ctx, cellSize, gravity, pipeArea, pipeLength);
	flowShader->render(ctx, orthoMesh);

	textures[1] = nextFluxMap->getShaderResourceView();

	// == 3rd pass - new water level calculation =================================================

	// update water heights using flux
	heightMap->setRenderTarget(ctx);
	waterShader->setShaderParameters(ctx, world, view, proj, textures);
	waterShader->setData(ctx, cellSize);
	waterShader->render(ctx, orthoMesh);
	
	textures[0] = heightMap->getShaderResourceView();

	// == 4th pass - water velocity calculation ==================================================

	velocityMap->setRenderTarget(ctx);
	velocityShader->setShaderParameters(ctx, world, view, proj, textures);
	velocityShader->setData(ctx, cellSize);
	velocityShader->render(ctx, orthoMesh);
	
	textures[2] = velocityMap->getShaderResourceView();

	// == 5th pass - erosion and deposition + evaporation ========================================

	nextHeightMap->setRenderTarget(ctx);
	erosionShader->setShaderParameters(ctx, world, view, proj, textures);
	erosionShader->setData(ctx, normalmap, cellSize, sedimentCap, dissolvingConst, depositionConst, evaporationConst);
	erosionShader->render(ctx, orthoMesh);

	textures[0] = nextHeightMap->getShaderResourceView();

	// == 6th pass - sediment transport ==========================================================

	heightMap->setRenderTarget(ctx);
	transportShader->setShaderParameters(ctx, world, view, proj, textures);
	transportShader->setData(ctx, cellSize);
	transportShader->render(ctx, orthoMesh);

	textures[0] = heightMap->getShaderResourceView();

	// Copy flux map back into buffer
	fluxMap->setRenderTarget(ctx);
	copyShader->setShaderParameters(ctx, world, view, proj, nextFluxMap->getShaderResourceView());
	copyShader->render(ctx, orthoMesh);
}

void Erosion::gui() {
	ImGui::Text("General");
	ImGui::Separator();

	ImGui::Checkbox("Simulate", &isSimulating);
	ImGui::Checkbox("Raining", &isRaining);

	ImGui::SliderFloat("Cell size X", &cellSize.x, 0.001f, 100.f);
	ImGui::SliderFloat("Cell size Y", &cellSize.y, 0.001f, 100.f);

	ImGui::Text("Rain");
	ImGui::Separator();

	ImGui::SliderFloat("Rain probability", &rainProbability, 0.001f, 1.f);
	ImGui::SliderFloat("Rain amount", &rainAmount, 0.001f, 1.f);

	ImGui::Text("Flux");
	ImGui::Separator();
	
	ImGui::SliderFloat("Gravity", &gravity, 0.1f, 20.f);
	ImGui::SliderFloat("Pipe area", &pipeArea, 0.001f, 100.f);
	ImGui::SliderFloat("Pipe length", &pipeLength, 0.001f, 100.f);

	ImGui::Text("Erosion");
	ImGui::Separator();
	
	ImGui::SliderFloat("Sediment capacity", &sedimentCap, 0.01f, 10.f);
	ImGui::SliderFloat("Dissolving constant", &dissolvingConst, 0.01f, 10.f);
	ImGui::SliderFloat("Deposition constant", &depositionConst, 0.01f, 10.f);
	ImGui::SliderFloat("Evaporation constant", &evaporationConst, 0.01f, 10.f);
}
