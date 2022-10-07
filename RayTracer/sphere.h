#pragma once
class Sphere
{
public:
	FColor color;
	int material = DIFFUSE;
	om::vec3 center;
	float radius;
	om::vec3 normal;

	Sphere(om::vec3 a_center, float a_radius, FColor a_color, int a_material) { CreateSphere(a_center, a_radius, a_color, a_material); }
	Sphere() : radius(0) {}

	void CreateSphere(om::vec3 a_center, float a_radius, FColor a_color, int a_material)
	{
		center = a_center;
		radius = a_radius;
		color = a_color;
		material = a_material;
		normal = om::normalize(center);
	}
};