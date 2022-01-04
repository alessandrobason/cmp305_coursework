Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0) {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer : register(b1) {
	float3 cameraPos;
	float timePassed;
}

struct InputType {
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 worldPos : WORLD_POS;
	float timePassed : TIME_PASSED;
};

OutputType main(InputType input) {
	OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	output.worldPos = mul(input.position, worldMatrix).xyz - cameraPos;

	output.timePassed = timePassed;

	return output;
}