#pragma once

#include <stdint.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include "ptr.h"

using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using int2 = DirectX::XMINT2;
using int3 = DirectX::XMINT3;
using int4 = DirectX::XMINT4;

using float2 = DirectX::XMFLOAT2;
using float3 = DirectX::XMFLOAT3;
using float4 = DirectX::XMFLOAT4;

using mat4 = DirectX::XMMATRIX;

using TextureType = ID3D11ShaderResourceView;
using Device = ID3D11Device;
using DeviceContext = ID3D11DeviceContext;
using Buffer = ID3D11Buffer;