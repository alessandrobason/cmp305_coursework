/**
* \class Base Mesh
*
* \brief The parent for all mesh objects. Provides default functionality.
*
* Can be inherited to create custom meshes. Provide functions for sending data to GPU memory, getting index count and storing geometry data.
*
* \author Paul Robertson
*/


#ifndef _BASEMESH_H_
#define _BASEMESH_H_

#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

class BaseMesh
{
protected:

	/// Default struct for general vertex data include position, texture coordinates and normals
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;

		VertexType() = default;
		VertexType(const XMFLOAT3 &pos, const XMFLOAT2 &text, const XMFLOAT3 &norm)
			: position(pos), texture(text), normal(norm) {}

	};

	/// Default vertex struct for geometry with only position and colour
	struct VertexType_Colour
	{
		XMFLOAT3 position;
		XMFLOAT4 colour;
	};

	/// Default vertex struct for geometry with only position and texture coordinates.
	struct VertexType_Texture
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	/// Empty constructor
	BaseMesh();
	~BaseMesh();

	/// Transfers mesh data to the GPU.
	virtual void sendData(ID3D11DeviceContext *deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	inline ID3D11Buffer *getVertexBuffer() { return vertexBuffer; }
	inline ID3D11Buffer *getIndexBuffer() { return indexBuffer; }

	inline void setVertexBuffer(ID3D11Buffer *newbuf) { vertexBuffer = newbuf; }
	inline void setIndexBuffer(ID3D11Buffer *newbuf) { indexBuffer = newbuf; }

	inline int getVertexCount() { return vertexCount; }
	inline int getIndexCount() { return indexCount; }

	inline void setVertexCount(int newcount) { vertexCount = newcount; }
	inline void setIndexCount(int newcount) { indexCount = newcount; }

	inline unsigned int getVertexSize() { return vertexSize; }

protected:
	virtual void initBuffers(ID3D11Device *) = 0;

	ID3D11Buffer *vertexBuffer, *indexBuffer;
	//D3D11_INPUT_ELEMENT_DESC *inputLayout;
	int vertexCount, indexCount;
	unsigned int vertexSize = sizeof(VertexType);
};

#endif