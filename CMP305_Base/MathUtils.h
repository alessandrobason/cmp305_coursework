#pragma once

#include <stdlib.h>
#include <DirectXMath.h>
#include "types.h"

#undef max
#undef min

template<typename T>
T degToRad(const T &deg) {
	return deg * 0.0174532925f;
}

template<typename T>
T radToDeg(const T &rad) {
	return rad * 57.2957795f;
}

template<typename T>
T max(const T &a, const T &b) {
	return a > b ? a : b;
}

template<typename T>
T min(const T &a, const T &b) {
	return a < b ? a : b;
}

template<typename T>
T clamp(const T &val, const T &min_val, const T &max_val) {
	return min(max(val, min_val), max_val);
}

template<typename T>
T pow2(const T &val) {
	return val * val;
}

template<typename T>
T randRange(const T &low, const T &high) {
	return low + T(rand()) / T(RAND_MAX / (high - low));
}

using namespace DirectX;

inline float4 mul(const mat4 &mat, const float4 &vec) {
	float4 res;
	XMStoreFloat4(
		&res,
		XMVector4Transform(
			XMLoadFloat4(&vec),
			mat
		)
	);
	return res;
}

inline float3 mul(const mat4 &mat, const float3 &vec) {
	float3 res;
	XMStoreFloat3(
		&res,
		XMVector4Transform(
			{ vec.x, vec.y, vec.z, 1.f },
			mat
		)
	);
	return res;
}

inline mat4 lookAt(const float3 &eyePos, const float3 &focusPos, const float3 &upDir) {
	return XMMatrixLookAtLH(
		XMLoadFloat3(&eyePos),
		XMLoadFloat3(&focusPos),
		XMLoadFloat3(&upDir)
	);
}