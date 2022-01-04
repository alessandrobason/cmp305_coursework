struct InputType {
	float4 position : SV_POSITION;
	float3 colour : COLOUR;
};

float4 main(InputType input) : SV_TARGET {
	return float4(input.colour, 1.0);
}



