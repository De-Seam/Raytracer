#pragma once
struct FColor
{
	float a, r, g, b;
	FColor(float r, float g, float b) : a(1), r(r), g(g), b(b) {}
	FColor(float a, float r, float g, float b) : a(a), r(r), g(g), b(b) {}
	FColor() : a(1), r(0), g(0), b(0) {}
	FColor(Pixel color)
	{
		float inverse = 1.0f / 255.0f;
		a = ((color >> 24) & 0xFF) * inverse;
		r = ((color >> 16) & 0xFF) * inverse;
		g = ((color >> 8) & 0xFF) * inverse;
		b = (color & 0xFF) * inverse;
	}

	Pixel HexColor()
	{
		float a_t, r_t, g_t, b_t;
		a_t = clamp(a, 0, 1);
		a_t = 1;
		r_t = clamp(r, 0, 1);
		g_t = clamp(g, 0, 1);
		b_t = clamp(b, 0, 1);
		return ((int)(a * 255) << 24) | ((int)(r_t * 255) << 16) | ((int)(g_t * 255) << 8) | (int)(b_t * 255);
	}

	Pixel AvgHexColor()
	{
		float total = r + g + b;
		return ((int)(1 * 255) << 24) | ((int)(r/total * 255) << 16) | ((int)(g / total * 255) << 8) | (int)(b / total * 255);
	}

	Pixel NormalHexColor()
	{
		om::vec3 temp(r, g, b);
		temp = om::normalize(temp);
		return ((int)(1 * 255) << 24) | ((int)(temp.x * 255) << 16) | ((int)(temp.y * 255) << 8) | (int)(temp.z * 255);
	}

	FColor operator - () const { return FColor(-a, -r, -g, -b); }
	FColor operator + (const FColor i) const { return FColor(a + i.a, r + i.r, g + i.g, b+ i.b); }
	FColor operator - (const FColor i) const { return FColor(a - i.a, r - i.r, g - i.g, b - i.b); }
	FColor operator * (const FColor i) const { return FColor(a * i.a, r * i.r, g * i.g, b * i.b); }
	FColor operator * (const float i) const { return FColor(a * i, r * i, g * i, b * i); }
	FColor operator / (const FColor i) const { return FColor(a / i.a, r / i.r, g / i.g, b / i.b); }
	FColor operator / (const float i) const { return FColor(a / i, r / i, g / i, b / i); }
	const FColor& operator += (const FColor i) { return *this = *this + i; }
	const FColor& operator -= (const FColor i) { return *this = *this - i; }
	const FColor& operator *= (const FColor i) { return *this = *this * i; }
	const FColor& operator *= (const float i) { return *this = *this * i; }
	const FColor& operator /= (const FColor i) { return *this = *this / i; }
	const FColor& operator /= (const float i) { return *this = *this / i; }
};