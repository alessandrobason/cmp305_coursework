#include "city.h"

#include <queue>

#include "stb_image.h"
#include "tracelog.h"
#include "MathUtils.h"

uchar *data = nullptr;
vec2i planeSize{ 100, 100 };
vec2i dataSize;

struct Pixel {
    u8 r, g, b, a;
};

// checks that the segment is compatible with all other segments, might modify it
static bool checkLocalConstraints(std::vector<RoadSegment> &list, RoadSegment &segment);
// uses global data (like population density) to generate new segments
static void applyGlobalGoals(std::priority_queue<RoadSegment> &list, RoadSegment &root);
static Pixel sample(vec2f uv);
static f32 sampleAlong(vec2f vector, vec2f dir, int samples, f32 len);
static vec2f rotate(vec2f vector, f32 radians);
static bool lineIntersects(vec2f a_start, vec2f a_end, vec2f b_start, vec2f b_end, vec2f &intersection);
static f32 dist2ToSegment(vec2f start, vec2f end, vec2f p, vec2f &closest);

std::vector<RoadSegment> buildCity(TextureIdManager &tmanager, int &textureId) {
    /*
    initialize priority queue Q with a single entry:
        r(0, ra, qa)

    initialize segment list S to empty

    until Q is empty
        pop smallest r(t, ra, qa) from Q
        compute (nqa, state) = localConstaints(qa)
        if(state == SUCCEED)
            add segment(ra) to S
            addZeroToThreeRoadsUsingGlobalGoals(Q, t+1, qa, ra)
    |
    |
    |
     --> where:
            r   is  a road segment
            t   is  time delay until segment is placed
            ra  is  geometrical properties of the segment
            qa  is  any addition metadata
    */

    int channels;
    data = stbi_load("res/pop_density.png", &dataSize.x, &dataSize.y, &channels, 4);

    textureId = tmanager.loadTexture(data, dataSize.x, dataSize.y, RGBA);
    info("x: %d y: %d channels: %d, id: %d", dataSize.x, dataSize.y, channels, textureId);

    std::vector<RoadSegment> accepted;

    std::priority_queue<RoadSegment> to_evaluate;
    to_evaluate.emplace(0, vec2f(0, 0), vec2f(-3, 0), HIGHWAY);
    to_evaluate.emplace(0, vec2f(0, 0), vec2f(3, 0), HIGHWAY);

    int i = 0;
    int limit = 300;

    while (i < limit && !to_evaluate.empty()) {
        auto currentSegment = to_evaluate.top();
        to_evaluate.pop();

        if (checkLocalConstraints(accepted, currentSegment)) {
            accepted.emplace_back(currentSegment);
            applyGlobalGoals(to_evaluate, currentSegment);
        }

        ++i;
    }

    return accepted;
}

bool checkLocalConstraints(
    std::vector<RoadSegment> &list, 
    RoadSegment &segment
) {
    constexpr f32 crossing_threshold = 1.f;
    constexpr f32 crossing_threshold2 = crossing_threshold * crossing_threshold;

    bool found = false;

    for (RoadSegment &road : list) {
        if (segment.start == road.start || segment.start == road.end) {
            continue;
        }

        vec2f point;

        // Check if they are overlapping, if so make a crossing
        if (lineIntersects(
              road.start, road.end, 
              segment.start, segment.end, 
              point)
        ) {
            segment.end = point;
            found = true;
            info("intersects");
            break;
        }

        // Otherwise, check if it is already close to a crossing
        if ((road.start - segment.end).mag2() < crossing_threshold2) {
            segment.end = road.start;
            found = true;
            info("start");
        }
        else if ((road.end - segment.end).mag2() < crossing_threshold2) {
            segment.end = road.end;
            found = true;
            info("end");
            break;
        }

        // Finally, check if it is close enough to extend it for a crossing
        f32 distance = dist2ToSegment(road.start, road.end, segment.end, point);
        if (distance < crossing_threshold) {
            segment.end = point;
            found = true;
            info("distance");
            break;
        }
    }

    if ((segment.end - segment.start).mag2() < 1.f) {
        if(found) info("too short (%.3f %.3f) (%.3f %.3f)", segment.start.x, segment.start.y, segment.end.x, segment.end.y);
        return false;
    }

    return true;
}

void applyGlobalGoals(
    std::priority_queue<RoadSegment> &list,
    RoadSegment &root
) {
    constexpr f32 r = 3;
    constexpr int samples = 5;

    constexpr f32 highway_threshold = 125;
    constexpr f32 highway_branch_prob = 5;

    constexpr f32 street_threshold = 125;
    constexpr f32 street_branch_prob = 40;

    constexpr f32 straight_range = 15.f;
    constexpr f32 branch_range   = 3.f;
    constexpr int segment_tries  = 5;

    constexpr int from_highway_delay = 5;

    vec2f dir = (root.end - root.start).normalized();

    f32 straight_pop_count = sampleAlong(root.end, dir, samples, r);

    if (root.type >= HIGHWAY) {
        // == POPULATION DENSITY ==============================
        vec2f lines[segment_tries];
        f32 maxFound = 0;
        int index = -1;

        f32 road_pop = straight_pop_count;

        // Shoot three rays of length [r] at a random angle between
        // (-45, +45), use the density map to calculate which one
        // is more "dense" by sampling the texture along the vector
        for (int i = 0; i < segment_tries; ++i) {
            f32 angle = degToRad(randRange(-straight_range, straight_range));
            vec2f next = rotate(dir, angle);

            // make the vector the length of r and add it to 
            // the end of the root for the final position
            lines[i] = root.end + next * r;

            // we want to keep [next] as it is a normalized 
            // vector in the direction of the new road segment

            f32 population = sampleAlong(root.end, next, samples, (f32)r);

            if (population >= maxFound) {
                maxFound = population;
                index = i;
            }
        }

        vec2f next_dir = dir;

        // if the population we found is more then just going straight,
        // push that, otherwise go straight
        if (maxFound > straight_pop_count) {
            assert(index != -1);
            list.emplace(0, root.end, lines[index], HIGHWAY);
            road_pop = maxFound;
            next_dir = (lines[index] - root.end).normalized();
        }
        else {
            list.emplace(0, root.end, root.end + dir * r, HIGHWAY);
        }

        // if the value at the population density is higher
        // then highway_threshold, try adding roads perpendicular 
        // to the highway
        if (road_pop >= highway_threshold) {
            if (randRange(0, 100) < highway_branch_prob) {
                // branch left
                f32 angle = degToRad(-90.f + randRange(-branch_range, branch_range));
                vec2f next = rotate(next_dir, angle);
                list.emplace(0, root.end, root.end + next * r, HIGHWAY_BRANCH);
            }            

            if (randRange(0, 100) < highway_branch_prob) {
                // branch right
                f32 angle = degToRad(90.f + randRange(-branch_range, branch_range));
                vec2f next = rotate(next_dir, angle);
                list.emplace(0, root.end, root.end + next * r, HIGHWAY_BRANCH);
            }
        }
    }
    else if (straight_pop_count >= street_threshold) {
        list.emplace(0, root.end, root.end + dir * r, STREET);
    }

    if (straight_pop_count >= street_threshold) {
        info("straight: %.3f", straight_pop_count);
        info("> (%.3f %.3f) (%.3f %.3f)\n", root.end.x, root.end.y, dir.x, dir.y);

        int t = root.type >= HIGHWAY ? from_highway_delay : 0;

        if (randRange(0, 100) < street_branch_prob) {
            // branch left
            f32 angle = degToRad(-90.f + randRange(-branch_range, branch_range));
            vec2f next = rotate(dir, angle);
            list.emplace(t, root.end, root.end + next * r, STREET_BRANCH);
        }

        if (randRange(0, 100) < street_branch_prob) {
            // branch right
            f32 angle = degToRad(90.f + randRange(-branch_range, branch_range));
            vec2f next = rotate(dir, angle);
            list.emplace(t, root.end, root.end + next * r, STREET_BRANCH);
        }
    }
    //else {
    //    info("failed: %.3f", straight_pop_count);
    //    info("> (%.3f %.3 f) (%.3f %.3f)\n", root.end.x, root.end.y, dir.x, dir.y);
    //}
}

Pixel sample(vec2f uv) {
    constexpr vec2f offset(50.f, 50.f);

    // flip on the y axis as in world space y+ is up
    // but in texture space y+ is down
    uv.y *= -1;

    // transform from word-space to texture-space
    uv = (uv + offset) * (vec2f)dataSize / (vec2f)planeSize;

    int x = (int)clamp(roundf(uv.x), 0.f, (f32)dataSize.x);
    int y = (int)clamp(roundf(uv.y), 0.f, (f32)dataSize.y);

    Pixel *pixels = (Pixel *)data;
    return pixels[x + y * dataSize.x];
}

f32 sampleAlong(vec2f vector, vec2f dir, int samples, f32 len) {
    f32 sum = 0;
    f32 step = len / (f32)samples;

    for (int s = 0; s < samples; ++s) {
        f32 dist = s * step;
        vec2f to_sample = vector + dir * dist;

        sum += sample(to_sample).r;
    }

    return sum / (f32)samples;
}

vec2f rotate(vec2f vector, f32 radians) {
    // rotate point around origin https://www.gamedev.net/forums/topic/286454-simple-2d-point-rotation/
    f32 c = cosf(radians);
    f32 s = sinf(radians);
    return {
        vector.x * c - vector.y * s,
        vector.x * s + vector.y * c
    };
}

// Determine the intersection point of two line segments
// Return FALSE if the lines don't intersect
// from http://paulbourke.net/geometry/pointlineplane/#i2l
bool lineIntersects(
    vec2f a_start, vec2f a_end,
    vec2f b_start, vec2f b_end,
    vec2f &intersection
) {
    float denom = ((b_end.y - b_start.y) * (a_end.x - a_start.x)) -
                  ((b_end.x - b_start.x) * (a_end.y - a_start.y));

    float nume_a = ((b_end.x - b_start.x) * (a_start.y - b_start.y)) -
                   ((b_end.y - b_start.y) * (a_start.x - b_start.x));

    float nume_b = ((a_end.x - a_start.x) * (a_start.y - b_start.y)) -
                   ((a_end.y - a_start.y) * (a_start.x - b_start.x));

    if (denom == 0.0f) {
        if (nume_a == 0.0f && nume_b == 0.0f) {
            // COINCIDENT;
            return false;
        }
        // PARALLEL;
        return false;
    }

    float ua = nume_a / denom;
    float ub = nume_b / denom;

    if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f) {
        // Get the intersection point.
        intersection.x = a_start.x + ua * (a_end.x - a_start.x);
        intersection.y = a_start.y + ua * (a_end.y - a_start.y);

        // INTERSECTING
        return true;
    }

    // NOT_INTERESECTING
    return false;
}

// from http://paulbourke.net/geometry/pointlineplane/
f32 dist2ToSegment(vec2f start, vec2f end, vec2f p, vec2f &closest) {
    vec2f delta = end - start;
    f32 denom = delta.mag2();
    assert(denom != 0);
    f32 u = ((p.x - start.x) * delta.x + (p.y - start.y) * delta.y) / denom;

    if (u < 0) {
        closest = start;
    }
    else if (u > 1) {
        closest = end;
    }
    else {
        closest = start + delta * u;
    }

    return (closest - p).mag2();
}