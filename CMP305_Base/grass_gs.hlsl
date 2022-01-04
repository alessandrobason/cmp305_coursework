Texture2D normalmap : register(t0);
SamplerState sampler0 : register(s0);

#include "noise.hlsl"

cbuffer MatrixBuffer : register(b0) {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer GrassBuffer : register(b1) {
	float3 cameraPosition;
	float slopeThreshold;
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
	float4 colour : COLOUR;
};

void addQuadCCW(
    float4 bottomLeft, 
    float4 bottomRight, 
    float4 topLeft, 
    float4 topRight, 
    float3 normal, 
    float4 colour, 
    inout TriangleStream<OutputType> triStream
) {
    OutputType output;
    
    // -- a front ---------------------------------------------------------------------------------------
    output.position = bottomLeft;
    output.tex = float2(0.0, 1.0);
	output.normal = normal;
	output.colour = colour;
    triStream.Append(output);

    output.position = bottomRight;
    output.tex = float2(1.0, 1.0);
	output.normal = normal;
	output.colour = colour;
    triStream.Append(output);

    output.position = topLeft;
    output.tex = float2(0.0, 0.0);
	output.normal = normal;
	output.colour = colour;
    triStream.Append(output);

    triStream.RestartStrip();

    // -- b front ---------------------------------------------------------------------------------------

    output.position = bottomRight;
    output.tex = float2(1.0, 1.0);
	output.normal = normal;
	output.colour = colour;
    triStream.Append(output);

    output.position = topRight;
    output.tex = float2(1.0, 0.0);
	output.normal = normal;
	output.colour = colour;
    triStream.Append(output);

    output.position = topLeft;
    output.tex = float2(0.0, 0.0);
	output.normal = normal;
	output.colour = colour;
    triStream.Append(output);

    triStream.RestartStrip();
}

void addQuadCW(
    float4 bottomLeft, 
    float4 bottomRight, 
    float4 topLeft, 
    float4 topRight, 
    float3 normal, 
    float4 colour, 
    inout TriangleStream<OutputType> triStream
) {
    OutputType output;
    
    // -- a back ----------------------------------------------------------------------------------------
    
    output.position = bottomLeft;
    output.tex = float2(0.0, 1.0);
	output.normal = normal;
	output.colour = colour;
    triStream.Append(output);

    output.position = topLeft;
    output.tex = float2(0.0, 0.0);
	output.normal = normal;
	output.colour = colour;
    triStream.Append(output);

    output.position = bottomRight;
    output.tex = float2(1.0, 1.0);
	output.normal = normal;
	output.colour = colour;
    triStream.Append(output);

    triStream.RestartStrip();
        

    // -- b back ---------------------------------------------------------------------------------------

    output.position = bottomRight;
    output.tex = float2(1.0, 1.0);
	output.normal = normal;
	output.colour = colour;
    triStream.Append(output);

    output.position = topLeft;
    output.tex = float2(0.0, 0.0);
	output.normal = normal;
	output.colour = colour;
    triStream.Append(output);

    output.position = topRight;
    output.tex = float2(1.0, 0.0);
	output.normal = normal;
	output.colour = colour;
    triStream.Append(output);

    triStream.RestartStrip();
}

void addGrass(InputType v1, InputType v2, float3 normal, inout TriangleStream<OutputType> triStream) {
    matrix mvp = mul(viewMatrix, projectionMatrix);

    float grassHeight = 1.0;
    float grassLen = 1.0;

    float4 point1 = v1.position;
    float4 point2 = v2.position;
	float4 norm = float4(normal, 0.0);
    float4 colour = float4(0.9, 0.9, 0.3, 1.0);
    float rand1 = random(v1.tex);
    float rand2 = random(v2.tex);
    
    // Randomize grassHeight
    grassHeight += rand2;

    // Randomize color
    colour.r -= rand1;
    colour.g -= rand2 * 0.3;
    colour = saturate(colour);
    
    float4 parallel = normalize(point2 - point1);
    float4 tangent = normalize(float4(cross(norm.xyz, parallel.xyz), 0.0));

    // Randomize rotation by changing first point's position along the tangent
	point1 += tangent * float4(rand1, 0, rand2, 0);
    parallel = normalize(point2 - point1);

    // Calculate all the points apart from bottomLeft using the
    // parallel and the tangent to make the height and length 
    // consisent
    float4 bottomLeft = point1;
    float4 bottomRight = bottomLeft + parallel * grassLen;
    float4 topLeft = bottomLeft + norm * grassHeight;

    // Move top left corner along the tangent so not all grass patches look
    // like they're pointing up, this makes the grass look more full
    topLeft += tangent * rand2;

    // Move the top left corner according to the wind
    // topLeft.xyz -= windDir * sin(timePassed * windSpeed + dist / waveAmplitude);
    
    // Use a normal vector parallel to topLeft/bottomLeft to calculate the
    // top right corner
    float4 verParallel = normalize(topLeft - bottomLeft);
    topLeft = bottomLeft + verParallel * grassHeight;
    float4 topRight = topLeft + parallel * grassLen;
    
    // We calculate the dot product of face normal (tangent) and the 
    // camera-to-bottomLeft vector, if the result is negative we display 
    // the anti-clockwise face, otherwise we render the clockwise face
    float4 horParallel = normalize(bottomLeft - bottomRight);
    
    // The tangent in this case is also the normal to the plane
    tangent = float4(cross(horParallel.xyz, verParallel.xyz), 1.0);

    float value = dot(tangent.xyz, bottomLeft.xyz - cameraPosition);

	bottomLeft  = mul(bottomLeft, mvp);
    bottomRight = mul(bottomRight, mvp);
    topLeft     = mul(topLeft, mvp);
    topRight    = mul(topRight, mvp);
    
	normal = normalize(tangent.xyz);
    
    if (value < 0) {
		addQuadCCW(bottomLeft, bottomRight, topLeft, topRight, normal, colour, triStream);
	}
    else {
		addQuadCW(bottomLeft, bottomRight, topLeft, topRight, normal, colour, triStream);
    }
}

[maxvertexcount(36)]
void main(triangle InputType input[3], inout TriangleStream<OutputType> triStream) {
    OutputType output;

	float2 uv = (input[0].tex + input[1].tex + input[2].tex) / 3;
    
	const float thresholdHigh = 1 - (slopeThreshold / 2);
	const float thresholdLow = thresholdHigh - slopeThreshold;
	float4 normalmapValue = normalmap.SampleLevel(sampler0, uv, 0);
	
    float3 normal = normalmapValue.xyz;
	float slope = normalmapValue.a;
    
	if (slope > thresholdLow) {
        addGrass(input[0], input[1], normal, triStream);
        addGrass(input[1], input[2], normal, triStream);
        addGrass(input[2], input[0], normal, triStream);
	}
}