/* Sky shader inspired by
 * https://www.patreon.com/posts/making-stylized-27402644
 */

Texture2D starsTexture : register(t0);
SamplerState sampler0 : register(s0);

cbuffer SkyBuffer : register(b0) {
	float3 lightDir;
	float padding;
	float4 sunCol;
	float4 moonCol;
	float4 dayBottomCol;
	float4 dayTopCol;
	float4 nightBottomCol;
	float4 nightTopCol;
	float4 sunsetCol;
	float radius;
	float moonOffset;
	float horizonIntensity;
	float starsExponent;
}

struct InputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 worldPos : WORLD_POS;
	float timePassed : TIME_PASSED;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse) {
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float getDisc(float3 worldPos, float3 lightDir, float radius) {
	float disc = distance(worldPos, lightDir);
	disc = 1 - saturate(disc / radius);
	disc = saturate(disc * 50.0);

	return disc;
}

float4 main(InputType input) : SV_TARGET{
	float3 lightDirection = lightDir * float3(1, -1, -1);
	float3 worldPos = input.worldPos;

	// Sun and moon silhouette
	float sunDisc = getDisc(worldPos, lightDirection, radius);
	float moonDisc = getDisc(worldPos, -lightDirection, radius);
	float crescentMoonDisc = getDisc(float3(worldPos.x + moonOffset, worldPos.yz), -lightDirection, 0.14);
	moonDisc = saturate(moonDisc - crescentMoonDisc);
	float4 sun = sunCol * sunDisc;
	float4 moon = moonCol * moonDisc;
	float4 sunMoon = sun + moon;
	
	// Sky color
	float4 gradientDay = lerp(dayBottomCol, dayTopCol, saturate(worldPos.y));
	float4 gradientNight = lerp(nightBottomCol, nightTopCol, saturate(worldPos.y));
	float4 skyGradients = lerp(gradientNight, gradientDay, saturate(lightDirection.y));

	// Sunset color
	float horizon = abs(worldPos.y * horizonIntensity);
	float sunset = saturate((1 - horizon) * saturate(lightDirection.y * 5));
	float4 sunsetColored = sunset * sunsetCol;
	
	// Stars
	float2 skyUV = (worldPos.xz / worldPos.y) + (input.timePassed / 20);
	float4 stars = starsTexture.Sample(sampler0, skyUV);
	stars *= 1 - saturate(lightDirection.y);
	stars = pow(abs(stars), starsExponent);
	
	return saturate(skyGradients + sunMoon + sunsetColored + stars);
}
