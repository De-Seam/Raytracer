#pragma once
class Triangle
{
public:
	FColor color;
	om::vec3 a, b, c, center;
	float metalReflectiveFactor = 0.f;
	int material = DIFFUSE;//default material
	Triangle(om::vec3 a, om::vec3 b, om::vec3 c, int material, FColor a_color) : a(a), b(b), c(c), material(material) { CalculateCenter(); color = a_color; }
	Triangle() {}
	void CalculateCenter();
	void Move(om::vec3 d);
	void Rotate(om::mat3 r);
	om::vec3 GetMaximum();
	om::vec3 GetMinimum();
};