#include "precomp.h"
#include "bvh.h"

float AABBox::Area() //Credits: https://box2d.org/files/ErinCatto_DynamicBVH_GDC2019.pdf
{
	om::vec3 temp = maxVal - minVal;
	return (2.0f * (temp.x * temp.y + temp.y * temp.z + temp.z * temp.x));
}

AABBox& AABBox::CreateBoundingBox(Vector<Triangle*>* triangleList)
{
	for (int j = 0; j < triangleList->size(); j++)
	{
		om::vec3 tempMax = triangleList->at(j)->GetMaximum();
		om::vec3 tempMin = triangleList->at(j)->GetMinimum();
		for (int k = 0; k < 3; k++)
		{
			maxVal.f[k] = max(tempMax.f[k], maxVal.f[k]);
			minVal.f[k] = min(tempMin.f[k], minVal.f[k]);
		}
	}
	return *this;
}

AABBox CreateBoundingBox(Vector<Triangle>* triangleList, int first, int count, unsigned int* indices)
{
	om::vec3 maxVal = -INFINITY;
	om::vec3 minVal = INFINITY;
	for (int j = first; j < first + count; j++)
	{
		om::vec3 tempMax = triangleList->at(indices[j]).GetMaximum();
		om::vec3 tempMin = triangleList->at(indices[j]).GetMinimum();
		for (int k = 0; k < 3; k++)
		{
			maxVal.f[k] = max(tempMax.f[k], maxVal.f[k]);
			minVal.f[k] = min(tempMin.f[k], minVal.f[k]);
		}
	}
	return AABBox(maxVal, minVal);
}

void BVH::Create(Vector<Triangle>* a_triangleList, int quality)
{
	this->triangleList = a_triangleList;
	//indices = new unsigned int[triangleList->size()];
	indices = (unsigned int*)_aligned_malloc((static_cast<size_t>(triangleList->size())+1)*4, 64);

	for (int i = 0; i < triangleList->size(); i++)
		indices[i] = i;

	Vector<BVHNode> a_node;
	a_node.reserve(a_triangleList->size() * 2 - 1);
	a_node.push_back(BVHNode());
	a_node.push_back(BVHNode());

	//root = new BVHNode();
	//node[1] reserven omdat cache lines node[2] node[3] 
	a_node[0].boundingBox = CreateBoundingBox(a_triangleList, 0, triangleList->size(), indices);
	a_node[0].Subdivide(triangleList, 0, triangleList->size(), 0, indices, &a_node, quality);
	
	node = (BVHNode*)_aligned_malloc((static_cast<size_t>(a_node.size()) + 1) * sizeof(BVHNode), 64);

	for (int i = 0; i < a_node.size(); i++)
		node[i] = a_node[i];

	double average = 0;
	int leafNodes = 0;
	for (int i = 0; i < a_node.size(); i++)
	{
		if (node[i].count > 0)
		{
			average += node[i].count;
			leafNodes++;
		}
	}
	average /= leafNodes;
	printf("Leaf nodes: %i\n", leafNodes);
	printf("Average triangles in leaf node: %f\n", average);
	printf("BVH Created with %i nodes\n", a_node.size());
}

void BVHNode::Subdivide(Vector<Triangle>* a_triangleList, int a_first, int a_count, int depth, unsigned int* indices, Vector<BVHNode>* node, int quality)
{
	boundingBox = CreateBoundingBox(a_triangleList, a_first, a_count, indices);
	this->Partition(a_triangleList, a_first, a_count, depth, indices, node, quality);
}

bool BVHNode::Partition(Vector<Triangle>* a_triangleList, int a_first, int a_count, int depth, unsigned int* indices, Vector<BVHNode>* node, int quality)
{
	//count = 0: leftFirst = next node
	//count > 0: leftFirst = first triangle

	//if (a_count < 3) {
	//	leftFirst = a_first;
	//	count = a_count;
	//	return false;
	//}

	om::vec3 sidesLength = boundingBox.maxVal - boundingBox.minVal;
	float bestSplitLocation = 0;
	float bestSplitCost = INFINITY;
	int axis = 0;

	int splitAmount = quality;
	if (a_count < splitAmount)
		splitAmount = a_count * 2;

	for (int xyz = 0; xyz < 3; xyz++)
	{
		float nextVal = 0;
		for (int i = 0; i < splitAmount; i++)
		{
			nextVal += sidesLength.f[xyz] / (float)splitAmount;
			float splitCost = this->ComputeSplitCost(a_triangleList, a_first, a_count, indices, boundingBox.minVal.f[xyz] + nextVal, xyz);
			if (splitCost < bestSplitCost) 
			{
				bestSplitCost = splitCost;
				bestSplitLocation = boundingBox.minVal.f[xyz] + nextVal;
				axis = xyz;
			}

		}
	}

	
	if (bestSplitCost >= boundingBox.Area() * (float)a_count)
	{
		leftFirst = a_first;
		count = a_count;
		return false;
	}

	int index = this->SplitIndices(a_triangleList, a_first, a_count, indices, bestSplitLocation, axis);
	int leftCount = index - a_first;

	leftFirst = node->size();

	node->push_back(BVHNode());
	node->push_back(BVHNode());

	node->at(leftFirst).Subdivide(a_triangleList, a_first, leftCount, depth + 1, indices, node, quality);
	node->at(leftFirst+1).Subdivide(a_triangleList, index, a_count- leftCount, depth + 1, indices, node, quality);
	return true;
}

float BVHNode::ComputeSplitCost(Vector<Triangle>* a_triangleList, int a_first, int a_count, unsigned int* indices, float splitLocation, int axis)
{
	int index = this->SplitIndices(a_triangleList, a_first, a_count, indices, splitLocation, axis);

	int triangleCountLeft = index - a_first;
	int triangleCountRight = a_count - triangleCountLeft;

	AABBox boundingBoxLeft = CreateBoundingBox(a_triangleList, a_first, triangleCountLeft, indices);
	AABBox boundingBoxRight = CreateBoundingBox(a_triangleList, index, triangleCountRight, indices);

	return boundingBoxLeft.Area() * triangleCountLeft + boundingBoxRight.Area() * triangleCountRight;
}

int BVHNode::SplitIndices(Vector<Triangle>* a_triangleList, int a_first, int a_count, unsigned int* indices, float splitLocation, int axis)
{
	//tested this with old code replacement, it works.
	int index = a_first;
	for (int j = a_first; j < a_first + a_count; j++)//add or remove -1?
	{
		//if(a_triangleList->at(indices[j]).a.f[axis] <= splitLocation || a_triangleList->at(indices[j]).b.f[axis] <= splitLocation || a_triangleList->at(indices[j]).c.f[axis] <= splitLocation)
		if(a_triangleList->at(indices[j]).center.f[axis] <= splitLocation)
		{
			//if(indices[index] != indices[j])
			om::swap(indices[index], indices[j]);
			index++;
		}
	}
	return index;
}

bool BVH::AnyHit(Ray ray, float& maxDistance)
{
	float t1, t2;
	if (!node[0].boundingBox.Intersect(ray, t1, t2))
		return false;

	return this->AnyHit(ray, maxDistance, 0);
}

bool BVH::AnyHit(Ray ray, float& maxDistance, int nodeNumber)
{
	if (node[nodeNumber].count > 0)
	{
		for (int i = node[nodeNumber].leftFirst; i < node[nodeNumber].leftFirst + node[nodeNumber].count; i++)
		{
			if (triangleList->at(indices[i]).material != GLASS && triangleList->at(indices[i]).material != FILLEDGLASS && ray.TriangleIntersection(triangleList->at(indices[i]), &maxDistance))
			{
				return true;
			}
		}
	}
	else
	{
		float d1, d2;
		return ((node[node[nodeNumber].leftFirst].boundingBox.Intersect(ray, d1, d2) && this->AnyHit(ray, maxDistance, node[nodeNumber].leftFirst)) || 
			(node[node[nodeNumber].leftFirst + 1].boundingBox.Intersect(ray, d1, d2) && this->AnyHit(ray, maxDistance, node[nodeNumber].leftFirst + 1)));
	}
	return false;

}
bool BVH::Intersect(Ray ray, intersectData& isectData, int& hit)
{
	hit++;
	if (!node[0].boundingBox.Intersect(ray))
		return false;

	return this->Intersect(ray, isectData, 0, hit);
}

bool BVH::Intersect(Ray ray, intersectData& isectData, int nodeNumber, int& hit)
{
	if (node[nodeNumber].count > 0)
	{
		bool returnValue = false;
		for (int i = node[nodeNumber].leftFirst; i < node[nodeNumber].leftFirst + node[nodeNumber].count; i++)
		{
			if (ray.TriangleIntersection(triangleList->at(indices[i]), &isectData.distance))
			{
				returnValue = true;
				isectData.triangle = &triangleList->at(indices[i]);
			}
		}
		if (returnValue)
			isectData.loc = ray.origin + ray.direction * isectData.distance;
		return returnValue;
	}
	else
	{
		float d1min, d1max, d2min, d2max;
		bool b_t1 = node[node[nodeNumber].leftFirst].boundingBox.Intersect(ray, d1min, d1max);
		bool b_t2 = node[node[nodeNumber].leftFirst + 1].boundingBox.Intersect(ray, d2min, d2max);
		hit += 2;

		if (b_t1 && b_t2)
		{
			if (d1min < d2min)
			{
				if (this->Intersect(ray, isectData, node[nodeNumber].leftFirst, hit))
				{
					if (d2min < isectData.distance)
						this->Intersect(ray, isectData, node[nodeNumber].leftFirst + 1, hit);
					return true;
				}
				else
					return this->Intersect(ray, isectData, node[nodeNumber].leftFirst + 1, hit
					);
			}
			else
			{
				if (this->Intersect(ray, isectData, node[nodeNumber].leftFirst + 1, hit))
				{
					if (d1min < isectData.distance)
						this->Intersect(ray, isectData, node[nodeNumber].leftFirst, hit);
					return true;
				}
				else
					return this->Intersect(ray, isectData, node[nodeNumber].leftFirst, hit);
			}
		}
		else if (b_t1)
		{
			return this->Intersect(ray, isectData, node[nodeNumber].leftFirst, hit);
		}
		else if (b_t2)
		{
			return this->Intersect(ray, isectData, node[nodeNumber].leftFirst + 1, hit);
		}
		return false;
	}
}

bool AABBox::Intersect(Ray ray)
{
	float t1, t2;
	return this->Intersect(ray, t1, t2);
}

bool AABBox::Intersect(Ray ray, float& tmin, float& tmax)//with thanks to scratchapixel
{
	om::vec3 inverseDirection(1.0f / ray.direction.x, 1.0f / ray.direction.y, 0.0f);

	if (inverseDirection.x >= 0) {
		tmin = (minVal.x - ray.origin.x) * inverseDirection.x;
		tmax = (maxVal.x - ray.origin.x) * inverseDirection.x;
	}
	else {
		tmin = (maxVal.x - ray.origin.x) * inverseDirection.x;
		tmax = (minVal.x - ray.origin.x) * inverseDirection.x;
	}

	float tymin, tymax;
	if (inverseDirection.y >= 0) {
		tymin = (minVal.y - ray.origin.y) * inverseDirection.y;
		tymax = (maxVal.y - ray.origin.y) * inverseDirection.y;
	}
	else {
		tymin = (maxVal.y - ray.origin.y) * inverseDirection.y;
		tymax = (minVal.y - ray.origin.y) * inverseDirection.y;
	}
	//if (tymin > tymax) om::swap(tymin, tymax);

	if (((tmin > tymax) || (tymin > tmax)))//add && tymin != tymax && tmin != tmax if more bugs present themselves //added last 2 && statements to bugfix edge cases 
		return false;

	
	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	inverseDirection.z = 1.0f / ray.direction.z;
	float tzmin, tzmax;
	if (inverseDirection.z >= 0) {
		tzmin = (minVal.z - ray.origin.z) * inverseDirection.z;
		tzmax = (maxVal.z - ray.origin.z) * inverseDirection.z;
	}
	else {
		tzmin = (maxVal.z - ray.origin.z) * inverseDirection.z;
		tzmax = (minVal.z - ray.origin.z) * inverseDirection.z;
	}

	if (tmin > tzmax || tzmin > tmax) //&& tzmin != tzmax
		return false;
	
	if (tzmin > tmin)
		tmin = tzmin;

	//if (tzmax < tmax)
	//	tmax = tzmax;
		
	//printf("%f : %f\n", tmin, tmax);
	return true;
}