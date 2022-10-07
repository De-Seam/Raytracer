#include "precomp.h"
#include "ray.h"

bool Ray::TriangleIntersection(Triangle triangle, float* distance)//credits: https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
{
	om::vec3 edge1 = triangle.b - triangle.a;
	om::vec3 edge2 = triangle.c - triangle.a;
	om::vec3 h = om::cross(this->direction, edge2);
	float a = dot(edge1, h);
	if (a > -EPSILON && a < EPSILON) //backface culling?
		return false;   // This ray is parallel to this triangle.
	float f = 1.0f / a;
	om::vec3 s = this->origin - triangle.a;
	float u = f * dot(s, h);
	if (u < 0.0 || u > 1.0)
		return false;
	om::vec3 q = cross(s, edge1);
	float v = f * dot(this->direction, q);
	if (v < 0.0 || u + v > 1.0)
		return false;
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * dot(edge2, q);

	//check if line intersection is also a ray intersection
	if (t > EPSILON) // ray intersection
	{
		if (t < *distance)
		{
			*distance = t;
			return true;
		}
		//printf("%i\n", triangleList->at(i).color);
	}
	return false;
}
bool Ray::SphereIntersection(Sphere sphere, float* distance)
{
	om::vec3 originToSphere = sphere.center - this->origin;
	//float projection = om::dot(originToSphere, this->direction);
	//om::vec3 distanceVector = originToSphere - direction * projection;

	float a = om::length2(this->direction);
	float b = 2.0f * om::dot(originToSphere, this->direction);
	float c = om::length2(originToSphere) - sphere.radius * sphere.radius;
	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0)
		return false;
	else
	{
		float temp = (-b - sqrtf(discriminant)) / (2.0f * a);
		if (temp < *distance && temp > 0)
		{
			*distance = temp;
			return true;
		}
	}

	return false;
}