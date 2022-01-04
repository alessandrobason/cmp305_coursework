#pragma once

#include <vector>
#include "vec.h"
#include "TextureIdManager.h"

enum {
	STREET, STREET_BRANCH, HIGHWAY, HIGHWAY_BRANCH
};

struct RoadSegment {
	int t = 0;
	vec2f start = vec2f::zero();
	vec2f end = { 1, 0 };
	int type = HIGHWAY;
	RoadSegment() = default;
	RoadSegment(int t, const vec2f &s, const vec2f &e, int type)
		: t(t), start(s), end(e), type(type) {}
};

inline bool operator<(const RoadSegment &a, const RoadSegment &b) {
	return a.t < b.t;
}

std::vector<RoadSegment> buildCity(TextureIdManager &tmanager, int &textureId);