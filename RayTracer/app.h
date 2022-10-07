#pragma once
class App;

struct ThreadData
{
	int ID;
	App* app;
	ThreadData() {}
	ThreadData(int ID, App* app) : ID(ID), app(app) {}
};

class App
{
public:
	void Init(Engine* a_engine);
	void Tick(double deltaTime);
	void KeyDown(int key);
	void KeyUp(int key);
	void CheckKeys(float deltaTime);
	void AddLightSource(Triangle a_triangle);
	void LoadObject(const char* filename, FColor color, float scale, int material);
	void LoadObject(const char* filename, FColor color, float scale, int material, om::vec3 transformation, om::vec3 rotation, float metalReflectiveFactor);
	Vector<Triangle> triangle;
	Vector<Triangle> lightSource;
	BVH bvh;
	SDL_Thread* thread[THREADCOUNT];
	ThreadData threadData[THREADCOUNT];
	om::vec3 camera, screenCenter, p0, p1, p2;
	float FOV;
	Engine* engine;
	FColor* pixelColor;
	int antiAliasing = 0, frame = 0;
	om::mat3 rotationMatrix;
	om::vec3 rotationVector;
	bool fastMode = false;
	int keyDown[256];

#ifdef TIMING_COUNT
	double timings[TIMING_COUNT];
	int timingIndex = 0;
#endif
};