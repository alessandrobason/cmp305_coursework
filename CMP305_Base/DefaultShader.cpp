#include "DefaultShader.h"

#include "utility.h"

// this values are taken from https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
AttenuationFactor lightFactors[] = {
	{ 1.f, 0.7f, 1.8f },
	{ 1.f, 0.35f, 0.44f },
	{ 1.f, 0.22f, 0.20f },
	{ 1.f, 0.14f, 0.07f },
	{ 1.f, 0.09f, 0.032f },
	{ 1.f, 0.07f, 0.017f },
	{ 1.f, 0.045f, 0.0075f },
	{ 1.f, 0.027f, 0.0028f },
	{ 1.f, 0.022f, 0.0019f },
	{ 1.f, 0.014f, 0.0007f },
	{ 1.f, 0.007f, 0.0002f },
	{ 1.f, 0.0014f, 0.000007f },
};

DefaultShader::DefaultShader(Device *device, HWND hwnd, bool init)
	: BaseShader(device, hwnd) {
	if (init) {
		initShader(L"light_vs.cso", L"light_ps.cso");
	}
}

DefaultShader::~DefaultShader() {
	SAFE_RELEASE(shadowMapSampler);
	SAFE_RELEASE(matrixBuffer);
}

void DefaultShader::setShaderParameters(
	DeviceContext *ctx, 
	const mat4 &world, 
	const mat4 &view, 
	const mat4 &proj, 
	TextureType *texture, 
	Light &light
) {
	// == MATRIX BUFFER =========================
	// Transpose the matrices to prepare them for the shader.
	mat4 tworld = XMMatrixTranspose(world);
	mat4 tview  = XMMatrixTranspose(view);
	mat4 tproj  = XMMatrixTranspose(proj);
	
	auto matrixPtr = MappedBuffer<MatrixBufferType>(ctx, matrixBuffer, VS, 0);
	matrixPtr->world = tworld;
	matrixPtr->view = tview;
	matrixPtr->projection = tproj;

	// == LIGHT BUFFER ==========================
	auto lightPtr = MappedBuffer<LightBufferType>(ctx, lightBuffer, PS, 0);
	lightPtr->diffuse   = light.getDiffuseColour();
	lightPtr->direction = light.getDirection();
	lightPtr->padding   = 0.f;

	// == SHADER RESOURCES ======================
	ctx->PSSetShaderResources(0, 1, &texture);
	ctx->PSSetSamplers(0, 1, &sampleState);
}

void DefaultShader::render(DeviceContext *ctx, MMesh &mesh, D3D11_PRIMITIVE_TOPOLOGY topology) {
	// == SEND DATA ==============================
	sendData(ctx, mesh, topology);

	// == RENDER =================================
	renderInternal(ctx, mesh);
	renderCleanup(ctx);
}

void DefaultShader::initDefaultBuffers() {
	addDynamicBuffer<MatrixBufferType>(&matrixBuffer);
	addDynamicBuffer<LightBufferType>(&lightBuffer);
}

void DefaultShader::addDiffuseSampler() {
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);
}

void DefaultShader::addShadowSampler() {
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &shadowMapSampler);
}

void DefaultShader::sendData(
	DeviceContext *ctx, 
	MMesh &mesh,
	D3D11_PRIMITIVE_TOPOLOGY topology
) {
	Buffer *vbuf = mesh.getVertexBuffer();
	Buffer *ibuf = mesh.getIndexBuffer();
	uint stride = mesh.getVertexSize();
	uint offset = 0;

	ctx->IASetVertexBuffers(0, 1, &vbuf, &stride, &offset);
	ctx->IASetIndexBuffer(ibuf, DXGI_FORMAT_R32_UINT, 0);
	ctx->IASetPrimitiveTopology(topology);
}

void DefaultShader::renderInternal(DeviceContext *ctx, MMesh &mesh) {
	// Set the vertex input layout.
	ctx->IASetInputLayout(layout);

	// Set the vertex and pixel shaders that will be used to render.
	ctx->VSSetShader(vertexShader, NULL, 0);
	if (isDepth) {
		ctx->PSSetShader(NULL, NULL, 0);
	}
	else {
		ctx->PSSetShader(pixelShader, NULL, 0);
	}
	ctx->CSSetShader(NULL, NULL, 0);

	// if Hull shader is not null then set HS and DS
	if (hullShader) {
		ctx->HSSetShader(hullShader, NULL, 0);
		ctx->DSSetShader(domainShader, NULL, 0);
	}
	else {
		ctx->HSSetShader(NULL, NULL, 0);
		ctx->DSSetShader(NULL, NULL, 0);
	}

	// if geometry shader is not null then set GS
	if (geometryShader) {
		ctx->GSSetShader(geometryShader, NULL, 0);
	}
	else {
		ctx->GSSetShader(NULL, NULL, 0);
	}

	// Render the triangle.
	ctx->DrawIndexed(mesh.getIndexCount(), 0, 0);
}

void DefaultShader::renderCleanup(DeviceContext *ctx) {
	static TextureType *nullTextures[5] = { 0 };
	ctx->VSSetShaderResources(0, ARR_LEN(nullTextures), nullTextures);
	ctx->HSSetShaderResources(0, ARR_LEN(nullTextures), nullTextures);
	ctx->DSSetShaderResources(0, ARR_LEN(nullTextures), nullTextures);
	ctx->GSSetShaderResources(0, ARR_LEN(nullTextures), nullTextures);
	ctx->PSSetShaderResources(0, ARR_LEN(nullTextures), nullTextures);
}

void DefaultShader::initShader(const wchar_t *vs, const wchar_t *ps) {
	loadVertexShader(vs);
	loadPixelShader(ps);
	//vertexShader = getDefaultVertexShader(this);
	//vertexDepthShader = getDefaultDepthShader(this);
	//pixelShader = getDefaultPixelShader(this);

	initDefaultBuffers();
	addDiffuseSampler();
	addShadowSampler();
}
