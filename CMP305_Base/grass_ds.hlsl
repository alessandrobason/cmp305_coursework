struct ConstantOutputType {
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
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

#define INTERPOLATE(var) patch[0].##var * uvwCoord.x + patch[1].##var * uvwCoord.y + patch[2].##var * uvwCoord.z

[domain("tri")]
OutputType main(
    ConstantOutputType input,
    float3 uvwCoord : SV_DomainLocation,
    const OutputPatch<InputType, 3> patch
) {
    OutputType output;

    output.position = INTERPOLATE(position);
    output.tex      = INTERPOLATE(tex);
    output.normal   = INTERPOLATE(normal);

    return output;
}
