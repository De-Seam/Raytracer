#include "precomp.h"
#include "triangle.h"

void Triangle::CalculateCenter()
{
	center = (a + b + c) / 3;
}

void Triangle::Move(om::vec3 d)
{
	a += d;
	b += d;
	c += d;
	center += d;
}

void Triangle::Rotate(om::mat3 r)
{
	a = r * a;
	b = r * b;
	c = r * c;
	center = (a + b + c) / 3;
}

om::vec3 Triangle::GetMaximum()
{
	om::vec3 returnvec;
	for (int i = 0; i < 3; i++)
	{
		returnvec.f[i] = max3(a.f[i], b.f[i], c.f[i]);
	}
	return returnvec;
}

om::vec3 Triangle::GetMinimum()
{
	om::vec3 returnvec;
	for (int i = 0; i < 3; i++)
	{
		returnvec.f[i] = min3(a.f[i], b.f[i], c.f[i]);
	}
	return returnvec;
}
/*
uint64_t zorder3d(uint64_t x, uint64_t y, uint64_t z) {
	static const uint64_t B[] = { 0x00000000FF0000FF, 0x000000F00F00F00F,
								   0x00000C30C30C30C3, 0X0000249249249249 };
	static const int S[] = { 16, 8, 4, 2 };
	static const uint64_t MAXINPUT = 65536;

	//assert(((x < MAXINPUT)) &&
	//	((y < MAXINPUT)) &&
	//	((z < MAXINPUT))
	//);

	x = (x | (x << S[0])) & B[0];
	x = (x | (x << S[1])) & B[1];
	x = (x | (x << S[2])) & B[2];
	x = (x | (x << S[3])) & B[3];

	y = (y | (y << S[0])) & B[0];
	y = (y | (y << S[1])) & B[1];
	y = (y | (y << S[2])) & B[2];
	y = (y | (y << S[3])) & B[3];

	z = (z | (z << S[0])) & B[0];
	z = (z | (z << S[1])) & B[1];
	z = (z | (z << S[2])) & B[2];
	z = (z | (z << S[3])) & B[3];

	return (x | (y << 1) | (z << 2));
}

void Triangle::CalculateMorton()// with thanks to http://www.graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN
{
	om::vec3 temp = om::cross(b - a, c - a);
	union
	{
		float tempX; 
		long int x;
	};
	union
	{
		float tempY;
		long int y;
	};
	union
	{
		float tempZ;
		long int z;
	};
	tempX = temp.x;
	tempY = temp.y;
	tempZ = temp.z;

	x = (x | (x << 16)) & 0x030000FF;
	x = (x | (x << 8)) & 0x0300F00F;
	x = (x | (x << 4)) & 0x030C30C3;
	x = (x | (x << 2)) & 0x09249249;

	y = (y | (y << 16)) & 0x030000FF;
	y = (y | (y << 8)) & 0x0300F00F;
	y = (y | (y << 4)) & 0x030C30C3;
	y = (y | (y << 2)) & 0x09249249;

	z = (z | (z << 16)) & 0x030000FF;
	z = (z | (z << 8)) & 0x0300F00F;
	z = (z | (z << 4)) & 0x030C30C3;
	z = (z | (z << 2)) & 0x09249249;

	morton = x | (y << 1) | (z << 2);

	uint64_t morton2 = zorder3d((uint64_t)temp.x, (uint64_t)temp.y, (uint64_t)temp.z);
}*/
/*
x: -111101110111010100011101100001
y: 1000000010100000100011111010000
z: 1000001101101110001000001001101

x: 1000001000000001001001001001
y: 1001001001000001000000000000
z: 1000001000000001001000001000

r: 11110011110000011101101001101
*/