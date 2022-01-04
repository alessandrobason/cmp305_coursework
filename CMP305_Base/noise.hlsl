float random(float2 st) {
	return frac(sin(dot(st, float2(12.9898, 78.233))) * 43758.5453123);
}

float perlin(float2 st) {
	float2 i = floor(st);
	float2 f = frac(st);

	// Get vectors for 4 corners
	float a = random(i);
	float b = random(i + float2(1, 0));
	float c = random(i + float2(0, 1));
	float d = random(i + float2(1, 1));
	
	// Cubic Hermine Curve
	float2 u = f * f * (3 - 2 * f);
	
	// Mix 4 coorners percentages
	return lerp(a, b, u.x) +
		   (c - a) * u.y * (1 - u.x) +
		   (d - b) * u.x * u.y;
}

float worley(float2 st) {
	float2 i = floor(st);
	float2 f = frac(st);
	
	float min_dist = 1;
	
	for (int y = -1; y <= 1; ++y) {
		for (int x = -1; x <= 1; ++x) {
			float2 neighbour = float2(x, y);
			float2 cur_point = random(i + neighbour);
			
			// (neighbour + cur_point) gets the relative 
			// current point position, f is the relative 
			// fragment position, we substract them to get
			// the vector inbetween them
			float2 diff = neighbour + cur_point - f;
			float dist = length(diff);
			min_dist = min(min_dist, dist);
		}
	}
	
	return min_dist;
}

#ifndef AMPLITUDE_CHANGE
#define AMPLITUDE_CHANGE 0.5
#endif

#ifndef COORD_CHANGE
#define COORD_CHANGE 2.0
#endif

#ifndef OCTAVES
#define OCTAVES 8
#endif

float fbmPerlin(float2 st) {
	float value = 0;
	float amplitude = AMPLITUDE_CHANGE;
	
	for (int i = 0; i < OCTAVES; ++i) {
		float n = perlin(st);
#ifdef USE_RIDGED_NOISE
		n = (n * 2) - 1; // convert to -1, 1 range
		n = 1 - abs(n); // make ridges and put them as the high values
		n = pow(n, 5); // sharpen creases
#elif defined(USE_POWERED_NOISE)
		n = 1 - abs(n); // make ridges and put them as the high values
		n = n * n; // sharpen creases
#endif
		value += amplitude * n;
		
		st *= COORD_CHANGE;
		amplitude *= AMPLITUDE_CHANGE;
	}
	
	return value;
}

float fbmWorley(float2 st) {
	float value = 0;
	float amplitude = AMPLITUDE_CHANGE;
	
	for (int i = 0; i < OCTAVES; ++i) {
		float n = worley(st);
#ifdef USE_RIDGED_NOISE
		n = (n * 2) - 1; // convert to -1, 1 range
		n = 1 - abs(n); // make ridges and put them as the high values
		n = pow(n, 5); // sharpen creases
#elif defined(USE_POWERED_NOISE)
		n = 1 - abs(n); // make ridges and put them as the high values
		n = n * n; // sharpen creases
#endif
		value += amplitude * n;
		
		st *= COORD_CHANGE;
		amplitude *= AMPLITUDE_CHANGE;
	}
	
	return value;
}