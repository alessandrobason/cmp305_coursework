// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
cbuffer MatrixBuffer : register(b0) {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct InputType {
	float4 position : POSITION;
	float3 colour : NORMAL;
};

struct OutputType {
	float4 position : SV_POSITION;
	float3 colour : COLOUR;
};

OutputType main(InputType input) {
	OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.colour = input.colour;

	return output;
}