#define HEIGHT_MAP(uv) (heightmap.Sample(sampler0, (uv)))
#define FLUX_MAP(uv) (fluxmap.Sample(sampler0, (uv)))
#define VELOCITY_MAP(uv) (velocitymap.Sample(sampler0, (uv)).xy)

#define TERRAIN(map)  (map.r)
#define WATER(map)	  (map.g)
#define SEDIMENT(map) (map.b)

#define FULL_HEIGHT(map)  (TERRAIN(map) + WATER(map))

#define LEFT(col)  (col.r)
#define RIGHT(col) (col.g)
#define UP(col)    (col.b)
#define DOWN(col)  (col.a)

#define COMPONENT_SUM(vec4) (vec4.x + vec4.y + vec4.z + vec4.w)

// fixed timestep
static const float dt = 1. / 60.;