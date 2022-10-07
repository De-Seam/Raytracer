#pragma once
om::vec3 ReflectionDirection(om::vec3 direction, om::vec3 normal);
om::vec3 RefractionDirection(om::vec3 intersectionPoint, om::vec3 normal, float eta);
FColor GetPixelColor(Ray ray, BVH *bvh, Vector<Triangle>* lightSource, int bounce, uint32_t* rngState, int lastMaterial, bool fastMode, int& hit);
