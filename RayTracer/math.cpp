#include "precomp.h"
#include "math.h"

namespace om
{
	//Credits: http://thompsonsed.co.uk/random-number-generators-for-c-performance-tested
	uint64_t random_seed{};
	/* The state can be seeded with any value. */
	void srand(uint64_t i)
	{
		random_seed = i;
	}

	uint64_t rand()
	{
		uint64_t z = (random_seed += UINT64_C(0x9E3779B97F4A7C15));
		z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
		z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
		return z ^ (z >> 31);
	}

	/*
	vec3 rotate(vec3 point, vec3 center, float theta)
	{
		float c = (float)cos(theta);
		float s = (float)sin(theta);

		return (point * c) + (cross(center, point) * s) + (center * dot(center, point)) * (1 - c);
	}*/

	// http://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
	uint32_t wang_hash(uint32_t seed)
	{
		seed = (seed ^ 61) ^ (seed >> 16);
		seed *= 9;
		seed = seed ^ (seed >> 4);
		seed *= 0x27d4eb2d;
		seed = seed ^ (seed >> 15);
		return seed;
	}

	uint32_t xorshift32(uint32_t* pstate)
	{
		// Xorshift algorithm from George Marsaglia's paper
		uint32_t state = *pstate;
		state ^= (state << 13);
		state ^= (state >> 17);
		state ^= (state << 5);
		return (*pstate) = state;
	}
	float xorshift32f(uint32_t* state)
	{
		return xorshift32(state) * (1.0f / 4294967296.0f);
	}
}