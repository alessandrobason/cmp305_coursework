#pragma once

#include "DXF.h"
#include "types.h"
#include "MModel.h"
#include "PixelOnlyShader.h"
#include "CopyShader.h"

class RainShader;
class FlowShader;
class WaterShader;
class VelocityShader;
class ErosionShader;
class TransportShader;

class Erosion {
public:
	void init(Device *device, DeviceContext *ctx, HWND hwnd, int mapSize, int screenWidth, int screenHeight);

	void update(DeviceContext *ctx, const mat4 &world, const mat4 &view, const mat4 &proj, TextureType *normalmap);
	void gui();

	inline TextureType *getHeightmap() { return heightMap->getShaderResourceView(); }
	inline RenderTexture *getHeightmapRT() { return heightMap; }

	inline TextureType *getFluxmap() { return fluxMap->getShaderResourceView(); }
	inline TextureType *getVelocitymap() { return velocityMap->getShaderResourceView(); }

	inline bool shouldSimulate() { return isSimulating; }

private:
	bool isSimulating = false;
	bool isRaining = false;
	MMesh orthoMesh;

	// Shader data
	float2 cellSize = float2(0.3f, 0.3f);
	float rainProbability  = 0.002f;
	float rainAmount       = 0.05f;
	float gravity          = 9.81f;
	float pipeArea         = 1.f;
	float pipeLength       = 0.3f;
	float sedimentCap      = 0.05f;
	float dissolvingConst  = 0.5f;
	float depositionConst  = 1.f;
	float evaporationConst = 0.05f;

	ptr<RenderTexture> heightMap;   // t1
	ptr<RenderTexture> fluxMap;     // t2
	ptr<RenderTexture> velocityMap; // t3

	ptr<RenderTexture> nextHeightMap;
	ptr<RenderTexture> nextFluxMap;

	ptr<CopyShader> copyShader;

	ptr<RainShader>      rainShader;
	ptr<FlowShader>      flowShader;
	ptr<WaterShader>     waterShader;
	ptr<VelocityShader>  velocityShader;
	ptr<ErosionShader>   erosionShader;
	ptr<TransportShader> transportShader;
};

class BaseErosionShader : public PixelOnlyShader {
public:
	BaseErosionShader(Device *device, HWND hwnd, const wchar_t *shader = nullptr);
	~BaseErosionShader();

	void setShaderParameters(DeviceContext *ctx, const mat4 &world, const mat4 &view, const mat4 &proj, TextureType *textures[3]);

protected:
	Buffer *buffer = nullptr;
};

class RainShader : public BaseErosionShader {
	struct BufferType {
		float2 cellSize;
		float rainProbability;
		float rainAmount;
		float2 seed;
		float2 padding;
	};
	static_assert(sizeof(BufferType) % 16 == 0, "BufferType is not 16 byte aligned");

public:
	RainShader(Device *device, HWND hwnd);
	void setData(DeviceContext *ctx, float2 cellSize, float prob, float amount, float2 seed);
};

class FlowShader : public BaseErosionShader {
	struct BufferType {
		float2 cellSize;
		float gravity;
		float pipeArea;
		float pipeLen;
		float3 padding;
	};
	static_assert(sizeof(BufferType) % 16 == 0, "BufferType is not 16 byte aligned");

public:
	FlowShader(Device *device, HWND hwnd);
	void setData(DeviceContext *ctx, float2 cellSize, float gravity, float area, float len);
};

class WaterShader : public BaseErosionShader {
	struct BufferType {
		float2 cellSize;
		float2 padding;
	};
	static_assert(sizeof(BufferType) % 16 == 0, "BufferType is not 16 byte aligned");

public:
	WaterShader(Device *device, HWND hwnd);
	void setData(DeviceContext *ctx, float2 cellSize);
};

class VelocityShader : public BaseErosionShader {
	struct BufferType {
		float2 cellSize;
		float2 padding;
	};
	static_assert(sizeof(BufferType) % 16 == 0, "BufferType is not 16 byte aligned");

public:
	VelocityShader(Device *device, HWND hwnd);
	void setData(DeviceContext *ctx, float2 cellSize);
};

class ErosionShader : public BaseErosionShader {
	struct BufferType {
		float2 cellSize;
		float sedimentCapacity;
		float dissolvingConst;
		float depositionConst;
		float evaporationConst;
		float2 padding;
	};
	static_assert(sizeof(BufferType) % 16 == 0, "BufferType is not 16 byte aligned");

public:
	ErosionShader(Device *device, HWND hwnd);
	void setData(DeviceContext *ctx, TextureType *normalmap, float2 cellSize, float capacity, float dissolving, float deposition, float evaporation);
};

class TransportShader : public BaseErosionShader {
	struct BufferType {
		float2 cellSize;
		float2 padding;
	};
	static_assert(sizeof(BufferType) % 16 == 0, "BufferType is not 16 byte aligned");

public:
	TransportShader(Device *device, HWND hwnd);
	void setData(DeviceContext *ctx, float2 cellSize);
};