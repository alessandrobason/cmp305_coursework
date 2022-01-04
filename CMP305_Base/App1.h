// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "LightShader.h"
#include "TerrainMesh.h"
#include "mmodel.h"
#include "TextureIdManager.h"
#include "DefaultShader.h"
#include "HeightShader.h"
#include "GroundShader.h"
#include "GrassShader.h"
#include "FlowerShader.h"

#include "vec.h"
#include "Erosion.h"
#include "Sky.h"
#include "LSystem.h"
//#include "StreetShader.h"
//#include "LineMesh.h"
//#include "city.h"


class App1 : public BaseApplication {
public:
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();

private:
	void buildHeightMap(DeviceContext *ctx, const mat4 &world, const mat4 &view, const mat4 &proj);
	void buildNormalMap(DeviceContext *ctx, const mat4 &world, const mat4 &view, const mat4 &proj);
	void buildRockTexture(DeviceContext *ctx, const mat4 &world, const mat4 &view, const mat4 &proj);
	void buildGrassTexture(DeviceContext *ctx, const mat4 &world, const mat4 &view, const mat4 &proj);
	void buildGrassPattern(DeviceContext *ctx, const mat4 &world, const mat4 &view, const mat4 &proj);

	void renderScene(const mat4 &world, const mat4 &view, const mat4 &proj);

	TextureIdManager tmanager;

	ptr<RenderTexture> flowerMap;
	//ptr<RenderTexture> heightMap;
	ptr<RenderTexture> normalMap;
	ptr<RenderTexture> rockMap;
	ptr<RenderTexture> grassTextureMap;
	ptr<RenderTexture> grassPatternMap;

	ptr<DefaultShader> shader;
	ptr<GroundShader> groundShader;
	ptr<HeightShader> heightShader;
	ptr<NormalShader> normalShader;
	ptr<GrassShader> grassShader;
	ptr<GrassPatternShader> grassPatShader;
	ptr<FlowerGenShader> flowerGenShader;
	ptr<FlowerShader> flowerShader;
	ptr<TextureGenShader> rockGenShader;
	ptr<TextureGenShader> grassGenShader;

	ptr<ShadowMap> shadowMap;

	Erosion erosion;
	Sky sky;

	ptr<MModel> grassModel;

	MMesh orthoMesh;
	MMesh plane;
	MMesh sphere;

	bool shouldUpdate = true;
	bool shouldUpdateTexture = true;

	Light light;

	int terrainResolution = 128;
	f32 timePassed = 0.f;
	f32 exponent = 1.f;
	f32 scale = 3.f;
	f32 maxHeight = 30.f;
	float2 offset = float2(0, 0);
	float slopeThreshold = 0.012f;
	float snowThreshold = 25.f;

	vec2f grassSeed;
	f32 grassPatScale = 1.f;
	f32 grassPatExp = 1.f;
	f32 grassPatThr = 1.f;

	f32 cutoffDistance = 40.f;
	f32 maxDist = 310.f;
	f32 maxFactor = 2.f;

	f32 flowerScale = 3.f;
	float4 flowerSeed;

	float4 rockLowCol  = float4(0.064f, 0.037f, 0.016f, 1.f);
	float4 rockHighCol = float4(0.597f, 0.367f, 0.016f, 1.f);
	float  rockScale   = 85.f;
	float  rockExp     = 0.4f;

	float4 grassLowCol  = float4(0.286f, 0.456f, 0.116f, 1.f);
	float4 grassHighCol = float4(0.836f, 0.856f, 0.116f, 1.f);
	float  grassScale   = 16.f;
	float  grassExp     = 1.f;
};

#endif