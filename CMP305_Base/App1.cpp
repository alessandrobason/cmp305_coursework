#include "App1.h"

#include <stdlib.h>
#include "utility.h"
#include "tracelog.h"
#include "vec.h"

constexpr f32 seedRange = 1000.f;

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN) {
	srand((uint)time(NULL));
	
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	Device *device = renderer->getDevice();
	DeviceContext *ctx = renderer->getDeviceContext();

	tmanager.init(device, ctx);

	// Load textures
	textureMgr->loadTexture(L"white", L"res/DefaultDiffuse.png");

	//line = new LineMesh(device, ctx);
	//quad = new QuadMesh(device, ctx);
	shader          = new DefaultShader(device, hwnd, true);
	heightShader    = new HeightShader(device, hwnd);
	normalShader    = new NormalShader(device, hwnd);
	groundShader    = new GroundShader(device, hwnd);
	grassShader     = new GrassShader(device, hwnd);
	grassPatShader = new GrassPatternShader(device, hwnd);
	flowerGenShader = new FlowerGenShader(device, hwnd);
	flowerShader    = new FlowerShader(device, hwnd);
	rockGenShader   = new TextureGenShader(device, hwnd, L"rock_gen_ps.cso");
	grassGenShader  = new TextureGenShader(device, hwnd, L"grass_gen_ps.cso");

	int textureSize = 1024;

	flowerMap = new RenderTexture(device, textureSize, textureSize, 0.1f, 100.f);
	//heightMap = new RenderTexture(device, textureSize, textureSize, 0.1f, 100.f);
	normalMap = new RenderTexture(device, textureSize, textureSize, 0.1f, 100.f);
	rockMap   = new RenderTexture(device, textureSize, textureSize, 0.1f, 100.f);
	grassTextureMap = new RenderTexture(device, textureSize, textureSize, 0.1f, 100.f);
	grassPatternMap = new RenderTexture(device, textureSize, textureSize, 0.1f, 100.f);

	shadowMap = new ShadowMap(device, textureSize, textureSize);
	erosion.init(device, ctx, hwnd, textureSize, screenWidth, screenHeight);
	sky.init(device, ctx, hwnd, &tmanager, &light);

	orthoMesh.moveFromMesh(new OrthoMesh(device, ctx, screenWidth, screenHeight));
	plane.moveFromMesh(new PlaneMesh(device, ctx, 200));
	sphere.moveFromMesh(new SphereMesh(device, ctx));

	light.generateOrthoMatrix((f32)sWidth, (f32)sHeight, 100.f, 1000.f);

	grassSeed = randRange(-100.f, 100.f);

	// generate 4 random value for the 4 flower maps
	flowerSeed.x = randRange(-seedRange, seedRange);
	flowerSeed.y = randRange(-seedRange, seedRange);
	flowerSeed.z = randRange(-seedRange, seedRange);
	flowerSeed.w = randRange(-seedRange, seedRange);

	MModelLoader loader;
	loader.init(device, &tmanager);

	grassModel = loader.load("res/grass.gltf");
	assert(grassModel);
}

App1::~App1() {
}

bool App1::frame() {
	if (!BaseApplication::frame())
		return false;

	timePassed += timer->getTime();
	
	if (!render())
		return false;

	return true;
}

bool App1::render() {
	DeviceContext *ctx = renderer->getDeviceContext();
	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	mat4 world = renderer->getWorldMatrix();
	mat4 view  = camera->getViewMatrix();
	mat4 proj  = renderer->getProjectionMatrix();
	mat4 orthoView = camera->getOrthoViewMatrix();
	mat4 orthoProj = renderer->getOrthoMatrix();

	bool isWireframe = renderer->getWireframeState();
	renderer->setWireframeMode(false);

	renderer->setZBuffer(false);

	if (shouldUpdate) {
		shouldUpdate = false;
		
		buildHeightMap(ctx, world, orthoView, orthoProj);
		buildNormalMap(ctx, world, orthoView, orthoProj);
	}

	if (shouldUpdateTexture) {
		shouldUpdateTexture = false;
		
		buildRockTexture(ctx, world, orthoView, orthoProj);
		buildGrassTexture(ctx, world, orthoView, orthoProj);
		buildGrassPattern(ctx, world, orthoView, orthoProj);
	}

	if (erosion.shouldSimulate()) {
		erosion.update(ctx, world, orthoView, orthoProj, normalMap->getShaderResourceView());
		buildNormalMap(ctx, world, orthoView, orthoProj);
	}

	renderer->setZBuffer(true);
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();

	// == Depth pass ===================================================

	light.generateViewMatrix();
	mat4 lightView = light.getViewMatrix();
	mat4 lightProj = light.getOrthoMatrix();

	groundShader->useDepth(true);

	shadowMap->BindDsvAndSetNullRenderTarget(ctx);
	renderScene(world, lightView, lightProj);

	groundShader->useDepth(false);

	// == Render pass ==================================================

	renderer->setZBuffer(true);
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
	
	renderer->setWireframeMode(isWireframe);

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	sky.render(renderer, camera, timePassed);

	renderScene(world, view, proj);

	/*
	static int grassTextureId = tmanager.loadTexture("res/grass.png");

	grassShader->setShaderParameters(
		ctx, world * planeMat, view, proj,
		heightMap->getShaderResourceView(),
		normalMap->getShaderResourceView(),
		tmanager.getTexture(grassTextureId),
		light,
		camera->getPosition(), slopeThreshold,
		cutoffDistance, maxDist, maxFactor
	);
	grassShader->render(ctx, plane);

	static int flowerTextureId = tmanager.loadTexture("res/red_flower.png");

	flowerShader->setShaderParameters(
		ctx, world * planeMat, view, proj,
		heightMap->getShaderResourceView(),
		normalMap->getShaderResourceView(),
		flowerMap->getShaderResourceView(),
		tmanager.getTexture(flowerTextureId),
		light,
		camera->getPosition(), slopeThreshold,
		cutoffDistance, maxDist, maxFactor
	);
	flowerShader->render(ctx, plane);
	*/

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();

	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	ImGui::Separator();

	ImGui::Text("Terrain");
	ImGui::Separator();

	ImGui::SliderFloat("Slope threshold", &slopeThreshold, 0.f, 0.2f);
	ImGui::SliderFloat("Snow threshold", &snowThreshold, 10.f, 80.f);

	shouldUpdate |= ImGui::Button("Reload");

	static ImVec2 imgsz{ 1024 / 5, 1024 / 5 };

	if (ImGui::CollapsingHeader("HeightMap")) {
		bool changedOff = false;
		changedOff |= ImGui::DragFloat("Map offset x", &offset.x, 0.1f);
		changedOff |= ImGui::DragFloat("Map offset y", &offset.y, 0.1f);
		shouldUpdate |= ImGui::SliderFloat("Map max height", &maxHeight, 1.f, 100.f);
		shouldUpdate |= ImGui::SliderFloat("Noise scale", &scale, 0.5f, 10.f);
		shouldUpdate |= ImGui::SliderFloat("Noise exponent", &exponent, 0.1f, 10.f);
		
		shouldUpdate |= changedOff;
		shouldUpdateTexture |= changedOff;

		ImGui::Image(erosion.getHeightmap(), imgsz);
	}

	if (ImGui::CollapsingHeader("NormalMap")) {
		ImGui::Image(normalMap->getShaderResourceView(), imgsz);
	}

	if (ImGui::CollapsingHeader("FluxMap")) {
		ImGui::Image(erosion.getFluxmap(), imgsz);
	}

	if (ImGui::CollapsingHeader("VelocityMap")) {
		ImGui::Image(erosion.getVelocitymap(), imgsz);
	}

	if (ImGui::CollapsingHeader("Erosion")) {
		erosion.gui();
	}

	if (ImGui::CollapsingHeader("Rock texture")) {
		shouldUpdateTexture |= ImGui::ColorEdit3("Rock low colour", (float *)&rockLowCol);
		shouldUpdateTexture |= ImGui::ColorEdit3("Rock high colour", (float *)&rockHighCol);
		shouldUpdateTexture |= ImGui::SliderFloat("Rock scale", &rockScale, 1.f, 1000.f);
		shouldUpdateTexture |= ImGui::SliderFloat("Rock exponent", &rockExp, 0.1f, 10.f);
		
		ImGui::Image(rockMap->getShaderResourceView(), imgsz);
	}

	if (ImGui::CollapsingHeader("Grass texture")) {
		shouldUpdateTexture |= ImGui::ColorEdit3("Grass low colour", (float *)&grassLowCol);
		shouldUpdateTexture |= ImGui::ColorEdit3("Grass high colour", (float *)&grassHighCol);
		shouldUpdateTexture |= ImGui::SliderFloat("Grass scale", &grassScale, 1.f, 1000.f);
		shouldUpdateTexture |= ImGui::SliderFloat("Grass exponent", &grassExp, 0.1f, 10.f);
		
		ImGui::Image(grassTextureMap->getShaderResourceView(), imgsz);
	}

	if (ImGui::CollapsingHeader("Grass pattern")) {
		if (ImGui::Button("Randomize")) {
			grassSeed = randRange(-100.f, 100.f);
			shouldUpdateTexture = true;
		}
		
		shouldUpdateTexture |= ImGui::SliderFloat("Scale", &grassPatScale, 0.1f, 20.f);
		shouldUpdateTexture |= ImGui::SliderFloat("Exponent", &grassPatExp, 0.1f, 10.f);
		shouldUpdateTexture |= ImGui::SliderFloat("Threshold", &grassPatThr, 0.001f, 1.f);
		
		ImGui::Image(grassPatternMap->getShaderResourceView(), imgsz);
	}

	if (ImGui::CollapsingHeader("Sky")) {
		sky.gui();
	}

	if (ImGui::CollapsingHeader("Shadow map")) {
		ImGui::Image(shadowMap->getDepthMapSRV(), imgsz);
	}

	// if (ImGui::Button("Print")) {
	// 	info("float4 groundLowCol  = float4(%.3ff, %.3ff, %.3ff, 1.f);", rockLowCol.x,  rockLowCol.y,  rockLowCol.z);
	// 	info("float4 groundHighCol = float4(%.3ff, %.3ff, %.3ff, 1.f);", rockHighCol.x, rockHighCol.y, rockHighCol.z);
	// 	info("float4 grassLowCol   = float4(%.3ff, %.3ff, %.3ff, 1.f);", grassLowCol.x,  grassLowCol.y,  grassLowCol.z);
	// 	info("float4 grassHighCol  = float4(%.3ff, %.3ff, %.3ff, 1.f);", grassHighCol.x, grassHighCol.y, grassHighCol.z);
	// }
	//ImGui::Separator();
	//ImGui::SliderFloat("Cutoff distance", &cutoffDistance, 1.f, 200.f);
	//ImGui::SliderFloat("Max distance", &maxDist, cutoffDistance, 500.f);
	//ImGui::SliderFloat("Max factor", &maxFactor, 1.f, 64.f);
	//ImGui::Separator();
	//ImGui::Image(flowerMap->getShaderResourceView(), { w, h });
	//ImGui::Text("Seed: %.3f %.3f %.3f %.3f", flowerSeed.x, flowerSeed.y, flowerSeed.z, flowerSeed.w);
	//if (ImGui::Button("Regenerate seed")) {
	//	flowerSeed.x = randRange(-seedRange, seedRange);
	//	flowerSeed.y = randRange(-seedRange, seedRange);
	//	flowerSeed.z = randRange(-seedRange, seedRange);
	//	flowerSeed.w = randRange(-seedRange, seedRange);
	//
	//	shouldUpdate = true;
	//}
	//ImGui::Separator();

	//ImGui::Checkbox("Update simulation", &updateSimulation);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void App1::buildHeightMap(
	DeviceContext *ctx, 
	const mat4 &world, 
	const mat4 &view, 
	const mat4 &proj
) {
	float2 resolution{ (f32)sWidth, (f32)sHeight };

	erosion.getHeightmapRT()->clearRenderTarget(ctx, 0.f, 0.f, 0.f, 1.f);
	erosion.getHeightmapRT()->setRenderTarget(ctx);
	heightShader->setShaderParameters(
		ctx,
		world, view, proj,
		resolution, exponent, scale, offset
	);
	heightShader->render(ctx, orthoMesh);
}

void App1::buildNormalMap(
	DeviceContext *ctx,
	const mat4 &world,
	const mat4 &view,
	const mat4 &proj
) {
	normalMap->clearRenderTarget(ctx, 0.f, 0.f, 0.f, 1.f);
	normalMap->setRenderTarget(ctx);
	normalShader->setShaderParameters(
		ctx, world, view, proj,
		erosion.getHeightmap(), maxHeight
	);
	normalShader->render(ctx, orthoMesh);
}

void App1::buildRockTexture(
	DeviceContext *ctx,
	const mat4 &world,
	const mat4 &view,
	const mat4 &proj
) {
	float2 resolution{ (f32)sWidth, (f32)sHeight };

	rockMap->clearRenderTarget(ctx, 0.f, 0.f, 0.f, 1.f);
	rockMap->setRenderTarget(ctx);
	rockGenShader->setShaderParameters(
		ctx, world, view, proj,
		resolution, rockScale, rockExp,
		rockLowCol, rockHighCol
	);
	rockGenShader->render(ctx, orthoMesh);

}

void App1::buildGrassTexture(
	DeviceContext *ctx,
	const mat4 &world,
	const mat4 &view,
	const mat4 &proj
) {
	float2 resolution{ (f32)sWidth, (f32)sHeight };

	grassTextureMap->clearRenderTarget(ctx, 0.f, 0.f, 0.f, 1.f);
	grassTextureMap->setRenderTarget(ctx);
	grassGenShader->setShaderParameters(
		ctx, world, view, proj,
		resolution, grassScale, grassExp,
		grassLowCol, grassHighCol
	);
	grassGenShader->render(ctx, orthoMesh);
}

void App1::buildGrassPattern(
	DeviceContext *ctx,
	const mat4 &world,
	const mat4 &view,
	const mat4 &proj
) {
	float2 resolution{ (f32)sWidth, (f32)sHeight };

	grassPatternMap->clearRenderTarget(ctx, 0.f, 0.f, 0.f, 1.f);
	grassPatternMap->setRenderTarget(ctx);
	grassPatShader->setShaderParameters(ctx, world, view, proj);
	grassPatShader->setData(ctx, resolution, grassSeed + offset, grassPatScale, grassPatExp, grassPatThr);
	grassPatShader->render(ctx, orthoMesh);
}

void App1::renderScene(const mat4 &world, const mat4 &view, const mat4 &proj) {
	DeviceContext *ctx = renderer->getDeviceContext();

	constexpr f32 planeScale = 3.f;
	constexpr f32 planeSize = 200.f;
	constexpr f32 planeOffset = -planeSize / 2.f;
	mat4 planeMat = XMMatrixTranslation(planeOffset, 0.f, planeOffset);
	planeMat *= XMMatrixScaling(planeScale, planeScale, planeScale);
	groundShader->setShaderParameters(
		ctx, world * planeMat, view, proj,
		erosion.getHeightmap(),
		normalMap->getShaderResourceView(),
		rockMap->getShaderResourceView(),
		grassTextureMap->getShaderResourceView(),
		shadowMap->getDepthMapSRV(),
		light, maxHeight, slopeThreshold, snowThreshold
	);
	groundShader->render(ctx, plane);
}
