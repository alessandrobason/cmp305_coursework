Texture2D heightmap : register(t0);
SamplerState sampler0 : register(s0);

cbuffer BufferType : register(b0) {
	float maxHeight;
	float3 padding;
};

struct InputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float getHeight(float2 uv) {
	float2 height = heightmap.SampleLevel(sampler0, uv, 0).rg;
	return height.r + height.g;
}

float3 getNormal(float2 uv, float2 texelSize) {
	float u = getHeight(uv + texelSize * float2( 0.0, -1.0)) * maxHeight;
	float r = getHeight(uv + texelSize * float2( 1.0,  0.0)) * maxHeight;
	float l = getHeight(uv + texelSize * float2(-1.0,  0.0)) * maxHeight;
	float d = getHeight(uv + texelSize * float2( 0.0,  1.0)) * maxHeight;
  
	float3 n;
	n.z = u - d;
	n.x = l - r;
	n.y = 2.0;
	
	return normalize(n);
}

float4 main(InputType input) : SV_TARGET {
	float2 texelSize;
	heightmap.GetDimensions(texelSize.x, texelSize.y);
	texelSize = 1 / texelSize;
	
	float3 normal = getNormal(input.tex, texelSize);
	
	float slope = dot(normal, float3(0, 1, 0));
	
	return float4(normal, slope);
}