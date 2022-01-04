Texture2D pattern : register(t0);
RWStructuredBuffer<float2> points : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	for (int i = 0; i < 10; ++i) {
		points[i] = float2(i, i * 10);
	}
}