#pragma once
struct RayReturnData
{
	float distance = INFINITY;
	om::vec3 intersectionPoint;
	int closestObject = -1;

	RayReturnData(float distance, om::vec3 intersectionPoint, int closestObject) : distance(distance), intersectionPoint(intersectionPoint), closestObject(closestObject) {}
	RayReturnData() {}
};

struct Ray
{
	om::vec3 origin, direction;

	Ray(om::vec3 origin, om::vec3 direction) : origin(origin), direction(direction) {}
	Ray() {}

	bool TriangleIntersection(Triangle triangle, float* distance);
	bool SphereIntersection(Sphere sphere, float* distance);
};