#pragma once

#include "DefaultShader.h"

struct SkyData {
	float3 lightDir = float3(0.f, 0.f, 0.f);
	float  padding = 0.f;
	float4 sunCol = float4(1.000f, 0.529f, 0.000f, 1.0);
	float4 moonCol = float4(1.000f, 0.862f, 0.529f, 1.0);
	float4 dayBottomCol = float4(0.230f, 0.730f, 0.580f, 1.f);
	float4 dayTopCol = float4(0.510f, 0.620f, 1.000f, 1.f);
	float4 nightBottomCol = float4(0.043f, 0.043f, 0.200f, 1.f);
	float4 nightTopCol = float4(0.086f, 0.004f, 0.004f, 1.f);
	float4 sunsetCol = float4(0.775f, 0.159f, 0.000f, 1.f);
	float  radius = 0.200f;
	float  moonOffset = 0.141f;
	float  horizonIntensity = 2.456f;
	float  starsExponent = 11.555f;
};

class SkyShader : public DefaultShader {
	struct BufferType {
		float3 cameraPos;
		float timePassed;
	};
public:
	SkyShader(Device *device, HWND hwnd);
	~SkyShader();

	void setShaderParameters(DeviceContext *ctx, const mat4 &world, const mat4 &view, const mat4 &projection, TextureType *texture, float3 cameraPos, float timePassed, Light *light, const SkyData &skyData);

private:
	ID3D11Buffer *buffer = nullptr;
	ID3D11Buffer *skyBuffer = nullptr;
};

// SkyMesh is just a sphere mesh with better uvs used for the sky box
struct SkyMesh : public MMesh {
	void init(Device *device, DeviceContext *ctx, int resolution = 20);
};

/* The sky class manages everything that regards the skybox,
 * from creating and rendering the skybox mesh to updating the
 * sunlight position.
 * The gui lets you print to console to easily copy-paste the
 * sky data in the SkyShader
 */
class Sky {
public:
	void init(Device *device, DeviceContext *ctx, HWND hwnd, TextureIdManager *textureManager, Light *dirlightPtr);

	void update(float dt);
	void render(D3D *renderer, FPCamera *camera, float timePassed);
	void gui();

	void setSunAngle(float deg);

private:
	void writeToConsole();

	ptr<SkyShader> shader;
	TextureIdManager *tmanager = nullptr;
	Light *sunlight = nullptr;
	SkyMesh mesh;

	SkyData skyData;
	f32 speed = 0.06f;
	bool is_paused = false;
};