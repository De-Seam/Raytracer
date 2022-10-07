#pragma once
struct intersectData
{
	om::vec3 loc;
	float distance = INFINITY;
	Triangle* triangle;
};

struct AABBox
{
	om::vec3 maxVal, minVal;
	AABBox(om::vec3 a_max, om::vec3 a_min) { SetBounds(a_max, a_min); }
	void SetBounds(om::vec3 a_max, om::vec3 a_min)
	{
		maxVal = a_max;
		minVal = a_min;
	}
	AABBox() : maxVal(-INFINITY), minVal(INFINITY) {}
	bool Intersect(Ray ray);
	bool Intersect(Ray ray, float& tmin, float& tmax);
	float Area();
	AABBox& CreateBoundingBox(Vector<Triangle*>* triangleList);
};

struct BVHNode
{
	AABBox boundingBox;
	int leftFirst = 0;
	int count = 0;

	void Subdivide(Vector<Triangle>* a_triangleList, int a_first, int a_count, int depth, unsigned int* indices, Vector<BVHNode>* node, int quality);
	bool Partition(Vector<Triangle>* a_triangleList, int a_first, int a_count, int depth, unsigned int* indices, Vector<BVHNode>* node, int quality);
	int SplitIndices(Vector<Triangle>* a_triangleList, int a_first, int a_count, unsigned int* indices, float splitLocation, int axis);
	float ComputeSplitCost(Vector<Triangle>* a_triangleList, int a_first, int a_count, unsigned int* indices, float splitLocation, int axis);
};

struct BVH
{
	Vector<Triangle>* triangleList;
	unsigned int* indices;
	BVHNode* node;

	BVH(Vector<Triangle>* a_triangleList) { this->Create(a_triangleList, 128); }
	BVH() {}
	void Create(Vector<Triangle>* a_triangleList, int quality);
	bool AnyHit(Ray ray, float& maxDistance);
	bool Intersect(Ray ray, intersectData& isectData, int& hit);
private:
	bool AnyHit(Ray ray, float& maxDistance, int nodeNumber);
	bool Intersect(Ray ray, intersectData& isectData, int nodeNumber, int& hit);
};