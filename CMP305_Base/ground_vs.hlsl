Texture2D heightmap : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0) {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer BufferType : register(b1) {
	float maxHeight;
	float3 padding;
	matrix lightMVP;
}

struct InputType {
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : SPOT_LIGHT_POS;
};

float getHeight(float2 uv) {
	float2 height = heightmap.SampleLevel(sampler0, uv, 0).rg;
	return height.r + height.g;
}

OutputType main(InputType input) {
	OutputType output;

	float4 pos = input.position;
	float3 norm = input.normal;
	
	pos.y += getHeight(input.tex) * maxHeight;
	
	float4 worldPosition = mul(pos, worldMatrix);
	
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(worldPosition, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
	output.normal = mul(norm, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);
	
	output.lightViewPos = mul(worldPosition, lightMVP);

	return output;
}