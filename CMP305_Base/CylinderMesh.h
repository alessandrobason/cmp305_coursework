#pragma once

#include "BaseMesh.h"
#include "types.h"

class CylinderMesh : public BaseMesh {
public:
	CylinderMesh(Device *device, DeviceContext *deviceContext, int stacks = 1, int slices = 6, float height = 1.0f, float bottomRadius = 0.5f, float topRadius = 0.5f);

protected:
	void initBuffers(Device *device) {}
	void init(Device *device, int stacks, int slices, float height, float bottomRadius, float topRadius);
};