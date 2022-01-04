#pragma once
#include <vector>

#include "mmodel.h"
#include "types.h"
#include "vec.h"

constexpr vec3f black   = vec3f(0, 0, 0);
constexpr vec3f blue    = vec3f(0, 0, 1);
constexpr vec3f green   = vec3f(0, 1, 0);
constexpr vec3f cyan    = vec3f(0, 1, 1);
constexpr vec3f red     = vec3f(1, 0, 0);
constexpr vec3f magenta = vec3f(1, 0, 1);
constexpr vec3f yellow  = vec3f(1, 1, 0);
constexpr vec3f white   = vec3f(1, 1, 1);

class LineSegment {
public:
	LineSegment()
		: start(0), end(0, 1), colour(1) {}
	
	LineSegment(const vec2f &s, const vec2f &e, const vec3f &c)
		: start(s), end(e), colour(c) {}

	vec2f start;
	vec2f end;
	vec3f colour;
};

class LineMesh : public BaseMesh {
	struct LineVertexType
	{
		float3 position;
		float3 colour;

		LineVertexType() = default;
		LineVertexType(const float2 &pos, const float3 &colour)
			: position(pos.x, pos.y, 0), colour(colour) {}
	};

public:
	LineMesh(Device *device, DeviceContext *deviceContext);

	void buildLine(DeviceContext *deviceContext, Device *device);

	inline const uint GetLineCount() { return (uint)lineList.size(); }

	//Line manipulation
	void addLine(const vec2f &start, const vec2f &end, const vec3f &colour) { lineList.emplace_back(start, end, colour); }
	void addSegment() { lineList.emplace_back(); }
	void clear() { lineList.clear(); }

private:
	std::vector<LineVertexType> vertices;
	std::vector<uint>           indices;
	std::vector<LineSegment>    lineList;

	void initBuffers(Device *device);
};