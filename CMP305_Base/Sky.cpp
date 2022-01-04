#include "Sky.h"

#include "utility.h"
#include "tracelog.h"
#include "MathUtils.h"
#include "vec.h"

SkyShader::SkyShader(Device *device, HWND hwnd)
	: DefaultShader(device, hwnd) {
	loadVertexShader(L"sky_vs.cso");
	loadPixelShader(L"sky_ps.cso");

	addDynamicBuffer<MatrixBufferType>(&matrixBuffer);
	addDynamicBuffer<BufferType>(&buffer);
	addDynamicBuffer<SkyData>(&skyBuffer);
	addDiffuseSampler();
}

SkyShader::~SkyShader() {
	SAFE_RELEASE(skyBuffer);
}

void SkyShader::setShaderParameters(
	DeviceContext *ctx,
	const mat4 &world,
	const mat4 &view,
	const mat4 &projection,
	TextureType *texture,
	float3 cameraPos,
	float timePassed,
	Light *light,
	const SkyData &skyData
) {
	// == MATRIX BUFFER =========================
	mat4 tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(world);
	tview = XMMatrixTranspose(view);
	tproj = XMMatrixTranspose(projection);
	auto matrix = MappedBuffer<MatrixBufferType>(ctx, matrixBuffer, VS, 0);
	matrix->world = tworld;
	matrix->view = tview;
	matrix->projection = tproj;

	// == CAMERA BUFFER =========================

	auto camPtr = MappedBuffer<BufferType>(ctx, buffer, VS, 1);
	camPtr->cameraPos = cameraPos;
	camPtr->timePassed = timePassed;

	// == SKY BUFFER =============================

	auto sky = MappedBuffer<SkyData>(ctx, skyBuffer, PS, 0);
	*sky = skyData;

	// Set shader texture resource in the pixel shader.
	ctx->PSSetShaderResources(0, 1, &texture);
	ctx->PSSetSamplers(0, 1, &sampleState);
}

void SkyMesh::init(Device *device, DeviceContext *ctx, int resolution) {
	VertexType *vertices = nullptr;
	ulong *indices = nullptr;
	D3D11_BUFFER_DESC vbufDesc{}, ibufDesc{};
	D3D11_SUBRESOURCE_DATA vdata{}, idata{};

	vertexCount = resolution * resolution * 24;
	indexCount = resolution * resolution * 36;

	vertices = new VertexType[vertexCount];
	indices = new ulong[indexCount];

	// Vertex variables
	float yincrement = 2.0f / resolution;
	float xincrement = 2.0f / resolution;
	float ystart = 1.0f;
	float xstart = -1.0f;
	//UV variables
	float txu = 0.0f;
	float txv = 0.0f;
	float txuinc = 1.0f / resolution;	// UV increment
	float txvinc = 1.0f / resolution;
	//Counters
	int v = 0;	// vertex counter
	int i = 0;	// index counter

	// == FRONT FACE ===============================================================

	for (int y = 0; y < resolution; y++) {
		for (int x = 0; x < resolution; x++) {
			// Load the vertex array with data.

			int startv = v;

			// -- BOTTOM LEFT ------------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, -1.0f);
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			++v;

			// -- TOP RIGHT --------------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart + xincrement, ystart, -1.0f);
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			++v;

			// -- TOP LEFT ---------------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart, ystart, -1.0f);
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			++v;

			// -- BOTTOM RIGHT -----------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart + xincrement, ystart - yincrement, -1.0f);
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			++v;

			// -- indices ----------------------------------------------------------

			indices[i + 0] = startv + 0;
			indices[i + 1] = startv + 2;
			indices[i + 2] = startv + 1;
			indices[i + 3] = startv + 0;
			indices[i + 4] = startv + 1;
			indices[i + 5] = startv + 3;

			i += 6;

			// increment
			xstart += xincrement;
			txu += txuinc;
		}

		ystart -= yincrement;
		xstart = -1;

		txu = 0;
		txv += txvinc;
	}

	txu = 0;
	txv = 0;


	// == BACK FACE ================================================================

	ystart = 1;
	xstart = 1;
	for (int y = 0; y < resolution; y++) {
		for (int x = 0; x < resolution; x++) {
			// Load the vertex array with data.

			int startv = v;

			// -- BOTTOM LEFT ------------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, 1.0f);
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			++v;

			// -- TOP RIGHT --------------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart - xincrement, ystart, 1.0f);
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			++v;

			// -- TOP LEFT ---------------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart, ystart, 1.0f);
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			++v;

			// -- BOTTOM RIGHT -----------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart - xincrement, ystart - yincrement, 1.0f);
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			++v;

			// -- indices ----------------------------------------------------------

			indices[i + 0] = startv + 0;
			indices[i + 1] = startv + 2;
			indices[i + 2] = startv + 1;
			indices[i + 3] = startv + 0;
			indices[i + 4] = startv + 1;
			indices[i + 5] = startv + 3;

			i += 6;

			// increment
			xstart -= xincrement;
			txu += txuinc;

		}

		ystart -= yincrement;
		xstart = 1;

		txu = 0;
		txv += txvinc;

	}

	txu = 1;
	txv = 0;

	// == RIGHT FACE ===============================================================

	ystart = 1;
	xstart = -1;
	for (int y = 0; y < resolution; y++) {
		for (int x = 0; x < resolution; x++) {
			// Load the vertex array with data.

			int startv = v;

			// -- BOTTOM LEFT ------------------------------------------------------

			vertices[v].position = XMFLOAT3(1.0f, ystart - yincrement, xstart);
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			++v;

			// -- TOP RIGHT --------------------------------------------------------

			vertices[v].position = XMFLOAT3(1.0f, ystart, xstart + xincrement);
			vertices[v].texture = XMFLOAT2(txu - txuinc, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			++v;

			// -- TOP LEFT ---------------------------------------------------------

			vertices[v].position = XMFLOAT3(1.0f, ystart, xstart);
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			++v;

			// -- BOTTOM RIGHT -----------------------------------------------------

			vertices[v].position = XMFLOAT3(1.0f, ystart - yincrement, xstart + xincrement);
			vertices[v].texture = XMFLOAT2(txu - txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			++v;

			// -- indices ----------------------------------------------------------

			indices[i + 0] = startv + 0;
			indices[i + 1] = startv + 2;
			indices[i + 2] = startv + 1;
			indices[i + 3] = startv + 0;
			indices[i + 4] = startv + 1;
			indices[i + 5] = startv + 3;

			i += 6;

			// increment
			xstart += xincrement;
			txu -= txuinc;
		}

		ystart -= yincrement;
		xstart = -1;
		txu = 1;
		txv += txvinc;
	}

	txu = 1;
	txv = 0;

	// == LEFT FACE ================================================================

	ystart = 1;
	xstart = 1;
	for (int y = 0; y < resolution; y++) {
		for (int x = 0; x < resolution; x++) {
			// Load the vertex array with data.

			int startv = v;

			// -- BOTTOM LEFT ------------------------------------------------------

			vertices[v].position = XMFLOAT3(-1.0f, ystart - yincrement, xstart);
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			++v;

			// -- TOP RIGHT --------------------------------------------------------

			vertices[v].position = XMFLOAT3(-1.0f, ystart, xstart - xincrement);
			vertices[v].texture = XMFLOAT2(txu - txuinc, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			++v;

			// -- TOP LEFT ---------------------------------------------------------

			vertices[v].position = XMFLOAT3(-1.0f, ystart, xstart);
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			++v;

			// -- BOTTOM RIGHT -----------------------------------------------------

			vertices[v].position = XMFLOAT3(-1.0f, ystart - yincrement, xstart - xincrement);
			vertices[v].texture = XMFLOAT2(txu - txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			++v;

			// -- indices ----------------------------------------------------------

			indices[i + 0] = startv + 0;
			indices[i + 1] = startv + 2;
			indices[i + 2] = startv + 1;
			indices[i + 3] = startv + 0;
			indices[i + 4] = startv + 1;
			indices[i + 5] = startv + 3;

			i += 6;

			// increment
			xstart -= xincrement;
			txu -= txuinc;
		}

		ystart -= yincrement;
		xstart = 1;
		txu = 1;
		txv += txvinc;
	}

	txu = 0;
	txv = 0;

	// == TOP FACE =================================================================

	ystart = 1;
	xstart = -1;

	for (int y = 0; y < resolution; y++) {
		for (int x = 0; x < resolution; x++) {
			// Load the vertex array with data.

			int startv = v;

			// -- BOTTOM LEFT ------------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart, 1.0f, ystart - yincrement);
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			++v;

			// -- TOP RIGHT --------------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart + xincrement, 1.0f, ystart);
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			++v;

			// -- TOP LEFT ---------------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart, 1.0f, ystart);
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			++v;

			// -- BOTTOM RIGHT -----------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart + xincrement, 1.0f, ystart - yincrement);
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			++v;

			// -- indices ----------------------------------------------------------

			indices[i + 0] = startv + 0;
			indices[i + 1] = startv + 2;
			indices[i + 2] = startv + 1;
			indices[i + 3] = startv + 0;
			indices[i + 4] = startv + 1;
			indices[i + 5] = startv + 3;

			i += 6;

			// increment
			xstart += xincrement;
			txu += txuinc;
		}

		ystart -= yincrement;
		xstart = -1;
		txu = 0;
		txv += txvinc;
	}

	txu = 0;
	txv = 0;

	// == BOTOTM FACE ==============================================================

	ystart = -1;
	xstart = -1;

	for (int y = 0; y < resolution; y++) {
		for (int x = 0; x < resolution; x++) {
			// Load the vertex array with data.

			int startv = v;

			// -- BOTTOM LEFT ------------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart, -1.0f, ystart + yincrement);
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
			++v;

			// -- TOP RIGHT --------------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart + xincrement, -1.0f, ystart);
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
			++v;

			// -- TOP LEFT ---------------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart, -1.0f, ystart);
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
			++v;

			// -- BOTTOM RIGHT -----------------------------------------------------

			vertices[v].position = XMFLOAT3(xstart + xincrement, -1.0f, ystart + yincrement);
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
			++v;

			// -- indices ----------------------------------------------------------

			indices[i + 0] = startv + 0;
			indices[i + 1] = startv + 2;
			indices[i + 2] = startv + 1;
			indices[i + 3] = startv + 0;
			indices[i + 4] = startv + 1;
			indices[i + 5] = startv + 3;

			i += 6;

			// increment
			xstart += xincrement;
			txu += txuinc;
		}

		ystart += yincrement;
		xstart = -1;
		txu = 0;
		txv += txvinc;
	}

	// now loop over every vertex and bend into a sphere (normalise the vertices)

	for (int counter = 0; counter < v; counter++) {
		vec3f v = vertices[counter].position;

		v.normalize();

		f64 angle_x = dot(v, { 1.f, 0.f, 0.f });

		f64 tex_u = angle_x / AI_MATH_HALF_PI;
		f64 tex_v = asin((f64)v.y) / AI_MATH_HALF_PI;

		vertices[counter].position = v;
		vertices[counter].normal = v;
		vertices[counter].texture = { (f32)tex_u, (f32)tex_v };
	}

	// Set up the description of the static vertex buffer.
	vbufDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vbufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufDesc.CPUAccessFlags = 0;
	vbufDesc.MiscFlags = 0;
	vbufDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vdata.pSysMem = vertices;
	vdata.SysMemPitch = 0;
	vdata.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vbufDesc, &vdata, &vertexBuffer);

	// Set up the description of the static index buffer.
	ibufDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	ibufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufDesc.CPUAccessFlags = 0;
	ibufDesc.MiscFlags = 0;
	ibufDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	idata.pSysMem = indices;
	idata.SysMemPitch = 0;
	idata.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&ibufDesc, &idata, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}

void Sky::init(Device *device, DeviceContext *ctx, HWND hwnd, TextureIdManager *textureManager, Light *dirlightPtr) {
	shader = new SkyShader(device, hwnd);
	tmanager = textureManager;

	mesh.init(device, ctx);
	mesh.textureId = tmanager->loadTexture("res/stars.jpg");

	sunlight = dirlightPtr;

	sunlight->setAmbientColour(0.2f, 0.1f, 0.05f, 1.f);
	sunlight->setDiffuseColour(1.f, 0.9f, 0.4f, 1.f);
	sunlight->setSpecularColour(0.044f, 0.035f, 0.018f, 1.f);
	sunlight->setSpecularPower(7.5f);

	setSunAngle(50.f);
}

void Sky::update(float dt) {
	/*
	if (is_paused) return;
	timePassed += dt;

	float3 pos = sunlight->getPosition();
	auto mat = XMMatrixRotationAxis({ 1.f, 0.f, 0.f }, dt * speed);
	XMStoreFloat3(&pos, XMVector3Transform(XMLoadFloat3(&pos), mat));
	vec3f dir = (vec3f::zero() - pos).normalized();
	sunlight->setPosition(pos.x, pos.y, pos.z);
	sunlight->setDirection(dir.x, dir.y, dir.z);
	*/
}

void Sky::render(D3D *renderer, FPCamera *camera, float timePassed) {
	mat4 world = renderer->getWorldMatrix();
	mat4 view = camera->getViewMatrix();
	mat4 proj = renderer->getProjectionMatrix();

	// put the skybox around the camera
	float3 cameraPos = camera->getPosition();
	world = world * XMMatrixTranslation(cameraPos.x, cameraPos.y, cameraPos.z);

	skyData.lightDir = sunlight->getDirection();

	renderer->setZBuffer(false);

	shader->setShaderParameters(
		renderer->getDeviceContext(),
		world, view, proj,
		tmanager->getTexture(mesh.textureId),
		cameraPos,
		timePassed,
		sunlight,
		skyData
	);
	shader->render(renderer->getDeviceContext(), mesh);

	renderer->setZBuffer(true);
}

#define COLOR_PICKER(name, val) ImGui::ColorEdit3(name, &skyData.##val##.x)

void Sky::gui() {
	ImGui::Checkbox("Pause", &is_paused);
	ImGui::SliderFloat("Speed", &speed, 0.001f, 2.f);
	COLOR_PICKER("Sun color", sunCol);
	COLOR_PICKER("Moon color", moonCol);
	COLOR_PICKER("Day bottom color", dayBottomCol);
	COLOR_PICKER("Day top color", dayTopCol);
	COLOR_PICKER("Night bottom color", nightBottomCol);
	COLOR_PICKER("Night top color", nightTopCol);
	COLOR_PICKER("Sunset color", sunsetCol);
	ImGui::SliderFloat("Radius", &skyData.radius, 0.01f, 0.3f);
	ImGui::SliderFloat("Crescent moon offset", &skyData.moonOffset, 0.01f, 0.3f);
	ImGui::SliderFloat("Horizon intensity", &skyData.horizonIntensity, 1.f, 10.0f);
	ImGui::SliderFloat("Stars exponent", &skyData.starsExponent, 1.f, 100.0f);

	static float angle = 0.f;
	if (ImGui::DragFloat("Sun angle", &angle)) {
		setSunAngle(angle);
	}

	if (ImGui::Button("Write to console")) {
		writeToConsole();
	}
}

void Sky::setSunAngle(float deg) {
	float3 pos{ 0, 600, 0 };
	mat4 mat = XMMatrixRotationAxis({ 1.f, 0.f, 0.f }, degToRad(deg));
	XMStoreFloat3(&pos, XMVector3Transform(XMLoadFloat3(&pos), mat));
	vec3f dir = (vec3f::zero() - pos).normalized();
	sunlight->setPosition(pos.x, pos.y, pos.z);
	sunlight->setDirection(dir.x, dir.y, dir.z);
}

void Sky::writeToConsole() {
	tall("struct SkyData {");
	tall("\tfloat3 lightDir         = float3(0.f, 0.f, 0.f);");
	tall("\tfloat  timePassed       = 0.f;");
	tall("\tfloat4 sunCol           = float4(%.3ff, %.3ff, %.3ff, 1.0);", skyData.sunCol.x, skyData.sunCol.y, skyData.sunCol.z);
	tall("\tfloat4 moonCol          = float4(%.3ff, %.3ff, %.3ff, 1.0);", skyData.moonCol.x, skyData.moonCol.y, skyData.moonCol.z);
	tall("\tfloat4 dayBottomCol     = float4(%.3ff, %.3ff, %.3ff, 1.f);", skyData.dayBottomCol.x, skyData.dayBottomCol.y, skyData.dayBottomCol.z);
	tall("\tfloat4 dayTopCol        = float4(%.3ff, %.3ff, %.3ff, 1.f);", skyData.dayTopCol.x, skyData.dayTopCol.y, skyData.dayTopCol.z);
	tall("\tfloat4 nightBottomCol   = float4(%.3ff, %.3ff, %.3ff, 1.f);", skyData.nightBottomCol.x, skyData.nightBottomCol.y, skyData.nightBottomCol.z);
	tall("\tfloat4 nightTopCol      = float4(%.3ff, %.3ff, %.3ff, 1.f);", skyData.nightTopCol.x, skyData.nightTopCol.y, skyData.nightTopCol.z);
	tall("\tfloat4 sunsetCol        = float4(%.3ff, %.3ff, %.3ff, 1.f);", skyData.sunsetCol.x, skyData.sunsetCol.y, skyData.sunsetCol.z);
	tall("\tfloat  radius           = %.3ff;", skyData.radius);
	tall("\tfloat  moonOffset       = %.3ff;", skyData.moonOffset);
	tall("\tfloat  horizonIntensity = %.3ff;", skyData.horizonIntensity);
	tall("\tfloat  starsExponent    = %.3ff;", skyData.starsExponent);
	tall("};");
}
