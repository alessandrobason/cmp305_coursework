#include "LineMesh.h"

#include "utility.h"
#include "tracelog.h"

LineMesh::LineMesh(Device *device, DeviceContext *deviceContext) {
	initBuffers(device);
}

void LineMesh::buildLine(DeviceContext *deviceContext, Device *device) {
	//Clear the vertex buffers
	SAFE_RELEASE(vertexBuffer);

	vertices.clear();
	indices.clear();

	vertexSize = sizeof(LineVertexType);

	indexCount = (int)lineList.size() * 2;

	float uvStep = 1.0f / indexCount;
	
	for (int i = 0; i < lineList.size(); i++) {
		auto &line = lineList[i];
		vec3f c = line.colour;
		c.x += randRange(-0.2f, 0.2f);
		c.y += randRange(-0.2f, 0.2f);
		c.z += randRange(-0.2f, 0.2f);

		vertices.emplace_back(
			(float2)line.start,
			(float3)c
		);

		vertices.emplace_back(
			(float2)line.end,
			(float3)c
		);

		indices.push_back(i * 2);
		indices.push_back(i * 2 + 1);
	}

	initBuffers(device);
}

// Generate buffers
void LineMesh::initBuffers(Device *device) {
	if (vertices.empty()) return;

	D3D11_BUFFER_DESC vertexBufferDesc{}, indexBufferDesc{};
	D3D11_SUBRESOURCE_DATA vertexData{}, indexData{};

	// Set up the description of the dyanmic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(LineVertexType) * (uint)vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = &vertices[0];
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(uint) * (uint)indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexData.pSysMem = &indices[0];

	indexCount = (int)indices.size();

	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
}