#include "precomp.h"
#include "raytracer.h"

om::vec3 ReflectionDirection(om::vec3 direction, om::vec3 normal)
{
	return direction - normal * om::dot(direction, normal) * 2;
}

om::vec3 RefractionDirection(om::vec3 intersectionPoint, om::vec3 normal, float eta)
{
	eta = 2.0f - eta;
	float N_dot_I = om::dot(normal, intersectionPoint);
	return (intersectionPoint * eta - normal * (-N_dot_I + eta * N_dot_I));
}

bool TraceShadowRay(Ray shadowRay, float& maxDistance, BVH* bvh)
{
	return bvh->AnyHit(shadowRay, maxDistance);
}

FColor GetPixelColor(Ray ray, BVH* bvh, Vector<Triangle>* lightSource, int bounce, uint32_t* rngState, int lastMaterial, bool fastMode, int& hit
)
{
	if(bounce > 6)//contribution is too low
		return FColor(1, 0, 1);

	intersectData isectData;
	if (!bvh->Intersect(ray, isectData, hit))
		return FColor(0);

	om::vec3 triangleNormal = om::normalize(om::cross(isectData.triangle->b - isectData.triangle->a, isectData.triangle->c - isectData.triangle->a));
	FColor returnColor(0);

	switch (isectData.triangle->material)
	{
	case DIFFUSE:
	{
		om::vec3 shadowRayOrigin = isectData.loc + triangleNormal * EPSILON;
		for (int i = 0; i < lightSource->size(); i++)
		{
			int lightSourcesHit = 0;
			float NdotLdivD = 0.f;
			for (int j = 0; j < SHADOWRAYAMOUNT; j++)
			{
				float rand1 = om::xorshift32f(rngState);
				float rand2 = om::xorshift32f(rngState);
				if (rand1 + rand2 > 1.0f)
				{
					rand1 = 1 - rand1;
					rand2 = 1 - rand2;
				}
				om::vec3 hitPoint = lightSource->at(i).a + (lightSource->at(i).b - lightSource->at(i).a) * rand1 + (lightSource->at(i).c - lightSource->at(i).a) * rand2;
				om::vec3 direction = hitPoint - shadowRayOrigin;
				float maxDistance = om::length(direction);
				om::vec3 directionNormal = direction/maxDistance;
				Ray shadowRay = Ray(shadowRayOrigin, directionNormal);
				if(!TraceShadowRay(shadowRay, maxDistance, bvh))
				{
					NdotLdivD += max(0.f, om::dot(triangleNormal, directionNormal)) / (maxDistance * maxDistance);
					lightSourcesHit++;
				}
			}
			FColor diffuse = lightSource->at(i).color * (NdotLdivD / (float)SHADOWRAYAMOUNT);
			returnColor += isectData.triangle->color * (diffuse + FColor(0.1f, 0.1f, 0.1f));//add Ambient lightning
		}
		returnColor /= (float)lightSource->size();
	}
	break;
	case METAL:
	{
		if (fastMode)
			return isectData.triangle->color;
		returnColor += GetPixelColor(Ray(isectData.loc + triangleNormal * EPSILON, ReflectionDirection(ray.direction, triangleNormal)), 
			bvh, lightSource, bounce + 1, rngState, MIRROR, fastMode, hit) * isectData.triangle->metalReflectiveFactor +
			isectData.triangle->color * (1 - isectData.triangle->metalReflectiveFactor);
	}
	break;
	case MIRROR:
	{
		if (fastMode)
			return isectData.triangle->color;
		returnColor += GetPixelColor(Ray(isectData.loc + triangleNormal * EPSILON, ReflectionDirection(ray.direction, triangleNormal)), 
			bvh, lightSource, bounce + 1, rngState, MIRROR, fastMode, hit);
	}
	break;
	case FILLEDGLASS: 
	case GLASS:
	{
		if (fastMode)
			return isectData.triangle->color;
		float reflect_factor = 0.1f;
		float refract_factor = 0.9f;
		if (lastMaterial != FILLEDGLASS)//Entering glass
			returnColor += GetPixelColor(Ray(isectData.loc + triangleNormal * EPSILON, ReflectionDirection(ray.direction, triangleNormal)), 
				bvh, lightSource, bounce + 1, rngState, isectData.triangle->material, fastMode, hit)* reflect_factor +
			GetPixelColor(Ray(isectData.loc, RefractionDirection(ray.direction, triangleNormal, 1.52f)), 
				bvh, lightSource, bounce + 1, rngState, isectData.triangle->material, fastMode, hit)* refract_factor;
		else //Exiting glass for beer's law
		{
			FColor absorbance = isectData.triangle->color * 0.15f * -isectData.distance;
			returnColor += GetPixelColor(Ray(isectData.loc, RefractionDirection(ray.direction, triangleNormal, 1.f/1.52f)), 
					bvh, lightSource, bounce + 1, rngState, GLASS, fastMode, hit) * refract_factor 
				* FColor(expf(absorbance.r), expf(absorbance.g), expf(absorbance.b));
		}
	}
	break;
	default:
		printf("ERROR: MATERIAL NOT FOUND: %i\n", isectData.triangle->material);
		break;
	}

	return returnColor;
}