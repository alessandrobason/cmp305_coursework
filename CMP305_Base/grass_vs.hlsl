Texture2D heightmap : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0) {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct InputType {
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType {
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float getHeight(float2 uv) {
	return heightmap.SampleLevel(sampler0, uv, 0).r;
}

OutputType main(InputType input) {
	OutputType output;
	
	const float maxHeight = 30;

	float4 pos = input.position;
	pos.y += getHeight(input.tex) * maxHeight;
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(pos, worldMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
	output.normal = 0;

	return output;
}