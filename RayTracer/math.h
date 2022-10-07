#pragma once
//#include <cstdint>
//---Copied from Jacco Bikker's lecture
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define clamp(x, a, b) max(min(x,b),a)
//---
#define min3(a, b, c) min(min(a, b), c)
#define max3(a, b, c) max(max(a, b), c)

#define UINT64_C(x)  (x ## ULL)

#pragma warning (push)
#pragma warning (disable : 4201) //to avoid nameless struct / union warning.

namespace om //own math
{
	struct vec3
	{
		union
		{
			struct
			{
				float x;
				float y;
				float z;
			};
			float f[3];
		};

		vec3(float x, float y, float z) : x(x), y(y), z(z) {}
		vec3(float a) : x(a), y(a), z(a) {}
		vec3() : x(0), y(0), z(0) {}

		float operator[] (int i) { return f[i]; }
		vec3 operator - () const { return vec3(-x, -y, -z); }
		vec3 operator + (const vec3 a) const { return vec3(x + a.x, y + a.y, z + a.z); }
		vec3 operator - (const vec3 a) const { return vec3(x - a.x, y - a.y, z - a.z); }
		vec3 operator * (const vec3 a) const { return vec3(x * a.x, y * a.y, z * a.z); }
		vec3 operator * (const float a) const { return vec3(x * a, y * a, z * a); }
		vec3 operator / (const vec3 a) const { return vec3(x / a.x, y / a.y, z / a.z); }
		vec3 operator / (const float a) const { return vec3(x / a, y / a, z / a); }
		const vec3& operator += (const vec3 a) { return *this = *this + a; }
		const vec3& operator -= (const vec3 a) { return *this = *this - a; }
		const vec3& operator *= (const vec3 a) { return *this = *this * a; }
		const vec3& operator *= (const float a) { return *this = *this * a; }
		const vec3& operator /= (const vec3 a) { return *this = *this / a; }
		const vec3& operator /= (const float a) { return *this = *this / a; }

		const void clear() { x = 0; y = 0; z = 0; return; }
	};

	inline float dot(vec3 a, vec3 b) { return (a.x * b.x) + (a.y * b.y) + (a.z * b.z); }
	inline vec3 cross(vec3 a, vec3 b) { return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
	inline float length2(vec3 a) { return dot(a, a); }
	inline float length(vec3 a) { return sqrtf(length2(a)); }
	inline vec3 normalize(vec3 a) { return a / length(a); }

	//vec3 rotate(vec3 point, vec3 center, float theta);



	struct vec4
	{
		union
		{
			struct
			{
				float x, y, z, w;
			};
			float f[4];
		};

		vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		vec4(float a) : x(a), y(a), z(a), w(a ){}
		vec4() : x(0), y(0), z(0), w(0) {}

		float& operator[](int i) { return f[i]; }
	};

	struct mat3
	{
		union
		{
			struct
			{
				vec3 x, y, z;
			};
			float f[3][3];
			vec3 col[3];
			float v[9];
		};

		mat3(vec3 i) : x(i), y(i), z(i) {}
		mat3(vec3 x, vec3 y, vec3 z) : x(x), y(y), z(z) {}
		mat3(float x0, float x1, float x2, float y0, float y1, float y2, float z0, float z1, float z2)
		{
			x = vec3(x0, x1, x2);
			y = vec3(y0, y1, y2);
			z = vec3(z0, z1, z2);
		}
		mat3() {}

		vec3& operator[](int i) { return col[i]; }
		vec3 operator * (const vec3 a) {
			return vec3(
				x.x* a.x + y.x * a.y + z.x * a.z,
				x.y* a.x + y.y * a.y + z.y * a.z,
				x.z* a.x + y.z * a.y + z.z * a.z);
		}

		mat3 operator * (const mat3 a) {
			return mat3(
				a.col[0]* f[0][0]+ a.col[1]* f[0][1]+
					a.col[2]* f[0][2],
				a.col[0]* f[1][0]+ a.col[1]* f[1][1]+
					a.col[2]* f[1][2],
				a.col[0]* f[2][0]+ a.col[1]* f[2][1]+
					a.col[2]* f[2][2]);
		}
	};

	inline mat3 rotateXY(float r)
	{
		float c = cosf(r);
		float s = sinf(r);
		return mat3(
			vec3(c, -s, 0),
			vec3(s, c, 0),
			vec3(0, 0, 1));
	}

	inline mat3 rotateXZ(float r)
	{
		float c = cosf(r);
		float s = sinf(r);
		return mat3(
			vec3(c, 0, s),
			vec3(0, 1, 0),
			vec3(-s, 0, c));
	}

	inline mat3 rotateYZ(float r)
	{
		float c = cosf(r);
		float s = sinf(r);
		return mat3(
			vec3(1, 0, 0),
			vec3(0, c, -s),
			vec3(0, s, c));
	}

	struct mat4 //made this before realizing I need a 3x3 matrix for camera rotation
	{
		union
		{
			struct
			{
				vec4 x, y, z, w;
			};
			float f[4][4];
			vec4 col[4];
			float a[16];
		};

		mat4(vec4 i) : x(i), y(i), z(i), w(i) {}
		mat4(vec4 x, vec4 y, vec4 z, vec4 w) : x(x), y(y), z(z), w(w) {}
		mat4(float x0, float x1, float x2, float x3, float y0, float y1, float y2, float y3, float z0, float z1, float z2, float z3, float w0, float w1, float w2, float w3)
		{
			x = vec4(x0, x1, x2, x3);
			y = vec4(y0, y1, y2, y3);
			z = vec4(z0, z1, z2, z3);
			w = vec4(w0, w1, w2, w3);
		}
		mat4() {}

		vec4& operator[](int i) { return col[i]; }
	};




	void srand(uint64_t i);
	uint64_t rand();

	uint32_t wang_hash(uint32_t seed);
	uint32_t xorshift32(uint32_t* pstate);
	float xorshift32f(uint32_t* state);

	template<typename T>
	void swap(T& a, T& b)
	{
		T c(a); a = b; b = c;
	}
}

#pragma warning (pop)