#pragma once

#include "SDL.h"
#undef main

#include <stdio.h>

typedef unsigned int Pixel;
#define SCRWIDTH 512
#define SCRHEIGHT 512
#define THREADCOUNT 10
#define EPSILON 0.0001f
#define PI 3.14159265358979323846264338327950288419716939f
#define AA 1 //anti aliasing per frame
#define SHADOWRAYAMOUNT 1 //amount of shadow rays for soft lightning
//#define HEATMAP 50.f //define this to enable heatmap which counts the amount of BVH intersection tests per pixel. The higher the amount the more green
//#define TIMEPERPIXEL 100.f //define this to enable heatmap with time per pixel. don't define HEATMAP at the same time though, that will cause errors. The lower the amount the more green.
//#define TIMING_COUNT 128 //define this to get last frame's time and an average time per frame

enum Material
{
	DIFFUSE = 0,
	MIRROR,
	GLASS, //use this to make the object hollow in the inside, only the walls are glass.
	FILLEDGLASS, //use this to fill a glass object and use beer's law
	METAL //metal has both reflection and it's own color mixed in.
};

#include "vector.h"
#include "math.h"
#include "timer.h"
#include "color.h"
#include "triangle.h"
#include "sphere.h"
#include "ray.h"
#include "bvh.h"
#include "file.h"
#include "engine.h"
#include "app.h"
#include "raytracer.h"