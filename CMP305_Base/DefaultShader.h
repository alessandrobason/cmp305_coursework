#pragma once

#include "DXF.h"
#include "types.h"
#include "mmodel.h"

using namespace std;
using namespace DirectX;

struct AttenuationFactor {
	float constant;
	float linear;
	float quadratic;
};

extern AttenuationFactor lightFactors[12];

enum {
	VS, DS, HS, GS, PS
};

template<typename T>
class MappedBuffer {
	T *data;
	DeviceContext *ctx;
	Buffer *buf;
	int stage;
	uint slot;

public:
	MappedBuffer(DeviceContext *ctx, Buffer *buffer, int stage, uint slot)
		: ctx(ctx), buf(buffer), stage(stage), slot(slot) {
		D3D11_MAPPED_SUBRESOURCE mappedResource{};
		HRESULT result = ctx->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		data = (T *)mappedResource.pData;
	}
	
	~MappedBuffer() {
		ctx->Unmap(buf, 0);
		switch (stage) {
			case VS: ctx->VSSetConstantBuffers(slot, 1, &buf); break;
			case DS: ctx->DSSetConstantBuffers(slot, 1, &buf); break;
			case HS: ctx->HSSetConstantBuffers(slot, 1, &buf); break;
			case GS: ctx->GSSetConstantBuffers(slot, 1, &buf); break;
			case PS: ctx->PSSetConstantBuffers(slot, 1, &buf); break;
		}
	}

	T *operator->() {
		return data;
	}

	T &operator*() {
		return *data;
	}
};

class DefaultShader : public BaseShader {
protected:
	struct LightBufferType {
		float4 diffuse;
		float3 direction;
		float padding;
	};

public:
	DefaultShader(Device *device, HWND hwnd, bool init = false);
	~DefaultShader();

	void setShaderParameters(
		DeviceContext *ctx,
		const mat4 &world, const mat4 &view, const mat4 &proj,
		TextureType *texture,
		Light &light
	);

	void render(DeviceContext *ctx, MMesh &mesh, D3D11_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	inline void useDepth(bool depth) { isDepth = depth; }

protected:
	void initShader(const wchar_t *vs, const wchar_t *ps);
	// Initializes default buffers
	void initDefaultBuffers();
	// Loads diffuse and shadow samplers
	void addDiffuseSampler();
	void addShadowSampler();

	void sendData(DeviceContext *ctx, MMesh &mesh, D3D11_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void renderInternal(DeviceContext *ctx, MMesh &mesh);
	void renderCleanup(DeviceContext *ctx);

	// Templated function to create dynamic buffer more easily using default options
	template<typename T>
	void addDynamicBuffer(Buffer **buf) {
		D3D11_BUFFER_DESC bufDesc{};
		bufDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufDesc.ByteWidth = sizeof(T);
		bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HRESULT res = renderer->CreateBuffer(&bufDesc, NULL, buf);
		//assert(SUCCESS(res));
	}

	template<typename T>
	void addStructuredBuffer(Buffer **buf, uint count) {
		D3D11_BUFFER_DESC bufDesc{};
		bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		bufDesc.ByteWidth = sizeof(T) * count;
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufDesc.StructureByteStride = sizeof(T);
		HRESULT res = renderer->CreateBuffer(&bufDesc, NULL, buf);
		//assert(SUCCESS(res));
	}

	ID3D11SamplerState *shadowMapSampler = nullptr;
	Buffer *lightBuffer = nullptr;
	bool isDepth = false;
};
