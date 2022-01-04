/**
* \class Base Shader
*
* \brief Loads shaders CSOs and sets parameters
*
* Base shader class to be inherited. Provides default functionality of loading and sending to GPU
* TODO: Add compute shader to set
* Base shader is the parent for other custom shader objects. Offers required functions and a standard Matrix buffer.
*
* \author Paul Robertson
*/


#ifndef _BASESHADER_H_
#define _BASESHADER_H_

#include <d3d11.h>
#include <D3Dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <fstream>
#include "imGUI/imgui.h"

using namespace std;
using namespace DirectX;


class BaseShader
{
protected:
	/** Default world, view, projection matrix buffer struct */
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	void *operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void *p)
	{
		_mm_free(p);
	}

	BaseShader(ID3D11Device *device, HWND hwnd);
	~BaseShader();

	/** \Brief render function
	* Sets shader stages and draws the indexed data
	*/
	virtual void render(ID3D11DeviceContext *deviceContext, int vertexCount);
	void compute(ID3D11DeviceContext *dc, int x, int y, int z);

	inline ID3D11VertexShader *getVertexShader() { return vertexShader; }
	inline ID3D11PixelShader *getPixelShader() { return pixelShader; }
	inline ID3D11HullShader *getHullShader() { return hullShader; }
	inline ID3D11DomainShader *getDomainShader() { return domainShader; }
	inline ID3D11GeometryShader *getGeometryShader() { return geometryShader; }
	inline ID3D11ComputeShader *getComputeShader() { return computeShader; }

	inline void setVertexShader(ID3D11VertexShader *newShader) { vertexShader = newShader; }
	inline void setPixelShader(ID3D11PixelShader *newShader) { pixelShader = newShader; }
	inline void setHullShader(ID3D11HullShader *newShader) { hullShader = newShader; }
	inline void setDomainShader(ID3D11DomainShader *newShader) { domainShader = newShader; }
	inline void setGeometryShader(ID3D11GeometryShader *newShader) { geometryShader = newShader; }
	inline void setComputeShader(ID3D11ComputeShader *newShader) { computeShader = newShader; }

protected:
	void loadVertexShader(const wchar_t *filename);		///< Load Vertex shader, for stand position, tex, normal geomtry
	void loadVertexShader(const wchar_t *filename, D3D11_INPUT_ELEMENT_DESC layoutDesc[], unsigned int layoutLen);
	void loadColourVertexShader(const wchar_t *filename);		///< Load Vertex shader, pre-made for position and colour only
	void loadTextureVertexShader(const wchar_t *filename);		///< Load Vertex shader, pre-made for position and tex only
	void loadHullShader(const wchar_t *filename);		///< Load Hull shader
	void loadDomainShader(const wchar_t *filename);		///< Load Domain shader
	void loadGeometryShader(const wchar_t *filename);	///< Load Geometry shader
	void loadPixelShader(const wchar_t *filename);		///< Load Pixel shader
	void loadComputeShader(const wchar_t *filename);	///< Load computer shader

	ID3DBlob *loadShader(const wchar_t *filename);

protected:
	ID3D11Device *renderer = nullptr;
	HWND hwnd;

	ID3D11VertexShader *vertexShader = nullptr;
	ID3D11PixelShader *pixelShader = nullptr;
	ID3D11HullShader *hullShader = nullptr;
	ID3D11DomainShader *domainShader = nullptr;
	ID3D11GeometryShader *geometryShader = nullptr;
	ID3D11ComputeShader *computeShader = nullptr;
	ID3D11InputLayout *layout = nullptr;
	ID3D11Buffer *matrixBuffer = nullptr;
	ID3D11SamplerState *sampleState = nullptr;
};

#endif