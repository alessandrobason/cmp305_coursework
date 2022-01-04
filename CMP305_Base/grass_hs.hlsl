struct InputType {
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct ConstantOutputType {
	float edges[3] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};

struct OutputType {
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

cbuffer HullBuffer : register(b0) {
	float3 cameraPos;
	float cutoffDistance;
	float maxDist;
	float maxFactor;
	float2 padding;
};

ConstantOutputType PatchConstantFunction(
	InputPatch<InputType, 3> inputPatch,
	uint patchId : SV_PrimitiveID
) {
	ConstantOutputType output;

	int tessellationFactor = maxFactor;

	// Calculate the average distance with the three points in the patch, if it
	// is less then the cutoff distance use the maximum tessellation factor, 
	// otherwise linearly decrase it until it reaces one

	float dist = 0.0;
	dist += distance(inputPatch[0].position.xz, cameraPos.xz);
	dist += distance(inputPatch[1].position.xz, cameraPos.xz);
	dist += distance(inputPatch[2].position.xz, cameraPos.xz);
	dist /= 3;

	if (dist > cutoffDistance) {
		tessellationFactor = maxFactor - (int)(dist * maxFactor / maxDist);
		tessellationFactor = clamp(tessellationFactor, 0, maxFactor);
	}

	// Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = tessellationFactor;
	output.edges[1] = tessellationFactor;
	output.edges[2] = tessellationFactor;

	// Set the tessellation factor for tessallating inside the triangle.
	output.inside = tessellationFactor;

	return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(
	InputPatch<InputType, 3> patch,
	uint pointId : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID
) {
	OutputType output;

	// Set the position for this control point as the output position.
	output.position = patch[pointId].position;
	output.tex      = patch[pointId].tex;
	output.normal   = patch[pointId].normal;

	return output;
}