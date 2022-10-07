#include "precomp.h"
#include "app.h"

void App::Init(Engine* a_engine)
{
	pixelColor = new FColor[SCRWIDTH * SCRHEIGHT];
	engine = a_engine;
	FOV = 1.2f;

	rotationVector = om::vec3(0, 0, 1);

	rotationMatrix = om::mat3(om::vec3(1.0f, 0.0f, 0), om::vec3(0, 1, 0), om::vec3(0, 0, 1));//identity matrix

	AddLightSource(Triangle(om::vec3(-180, 410, 0), om::vec3(-200, 400, 0), om::vec3(-220, 390, 0), DIFFUSE, FColor(100000, 100000, 100000)));//you will always get a little bit of the original color because of ambient lighting, even when setting one of the light RGB values to 0
	AddLightSource(Triangle(om::vec3(-80, 410, 300), om::vec3(-100, 400, 300), om::vec3(-120, 390, 300), DIFFUSE, FColor(100000, 100000, 100000)));

	camera = om::vec3(-252, 252, -300);

#ifdef TIMING_COUNT
	for (int i = 0; i < TIMING_COUNT; ++i) 
		timings[i] = 0.0;
#endif

	LoadObject("roomwhite.obj", 0xFFFFFF, 1, DIFFUSE); //white part of the room
	LoadObject("roomgreen.obj", 0x00FF00, 1, DIFFUSE); //green wall
	LoadObject("roomred.obj", 0xFF0000, 1, DIFFUSE); //red wall
	LoadObject("cube.obj", FColor(0x000000), 1.f, FILLEDGLASS, om::vec3(-10, 10, 0), om::vec3(0, 0, 0), 0.f); //glass cube below bunny
	LoadObject("cuboid.obj", 0x00FFFF, 1.f, MIRROR); //big mirror
	LoadObject("bunny.obj", 0xAAAAAA, 1, DIFFUSE); //standard gray bunny
	//LoadObject("bunny.obj", 0xD4AF37, 1, METAL, om::vec3(0, 0, 0), om::vec3(0, 0, 0), 0.4f);//nice looking gold metal bunny
	//LoadObject("dragon.obj", 0xAAAAAA, 300, DIFFUSE, om::vec3(-100, 300, 150), om::vec3(PI/2, 0, 0), 0.f);//buggy stanford dragon, I had to manually replace all 'e's in the obj file because my file loader doesn't recognize it and I probably made a typo somewhere. 

	printf("Objects loaded\nTriangle count: %i\n", triangle.size());
	bvh.Create(&triangle, 128);//the higher the quality integer the better the BVH, but it will also take longer to construct the BVH.

	for (int i = 0; i < THREADCOUNT; i++)//i = ID of the thread, which determines the pixels each thread traces.
		threadData[i] = ThreadData(i, this);

	for (int i = 0; i < 256; i++)
		keyDown[i] = false;
}

void App::AddLightSource(Triangle a_triangle)
{
	lightSource.push_back(a_triangle);
}

void App::LoadObject(const char* filename, FColor color, float scale, int material, om::vec3 transformation, om::vec3 rotation, float metalReflectiveFactor)
{
	int oldSize = triangle.size();
	LoadObj(filename, &triangle, color, scale, material);
	if (rotation.x != 0 || rotation.y != 0 || rotation.z != 0)
	{
		om::mat3 a_rotationMatrix = om::rotateXZ(rotation.y) * om::rotateYZ(rotation.x) * om::rotateXY(rotation.z);
		for (int i = oldSize; i < triangle.size(); i++)
		{
			triangle[i].Rotate(a_rotationMatrix);
		}
	}
	if (transformation.x != 0 || transformation.y != 0 || transformation.z != 0)
	{
		for (int i = oldSize; i < triangle.size(); i++)
		{
			triangle[i].Move(transformation);
		}
	}
	if (material == METAL && metalReflectiveFactor > 0)
	{
		for (int i = oldSize; i < triangle.size(); i++)
		{
			triangle[i].metalReflectiveFactor = metalReflectiveFactor;
		}
	}
	printf("Object loaded: %s\n", filename);
}

void App::LoadObject(const char* filename, FColor color, float scale, int material)
{
	this->LoadObject(filename, color, scale, material, om::vec3(0, 0, 0), om::vec3(0, 0, 0), 0.f);
}

int RenderSection(void* ptr)
{
	ThreadData* data = (ThreadData*)ptr;
	float inverseScreenHeight = 1.0f / SCRHEIGHT;
	float inverseScreenWidth = 1.0f / SCRWIDTH;
	om::vec3 width = om::vec3(data->app->p1 - data->app->p0);
	om::vec3 height = om::vec3(data->app->p2 - data->app->p0);
	for (int y = data->ID; y < SCRHEIGHT; y += THREADCOUNT)
	{
		int yTimesScrWidth = y * SCRWIDTH;
		unsigned int* lineStart = data->app->engine->pixels + yTimesScrWidth;
		FColor* lineStartColor = data->app->pixelColor + yTimesScrWidth;
		for (int x = 0; x < SCRWIDTH; x++)
		{
#ifdef TIMEPERPIXEL
			Timer timer;
			timer.StartCounter();
#endif
			int currentAA = AA;
			FColor pixelColor;
			uint32_t rngState = om::wang_hash(x + y * SCRWIDTH + data->app->frame);
			for (int i = 0; i < currentAA; i++)
			{
				float v = (float)(y + om::xorshift32f(&rngState)) * inverseScreenHeight;
				float u = (float)(x + om::xorshift32f(&rngState)) * inverseScreenWidth;
				om::vec3 screenLocation = data->app->p0 + width * u + height * v;
				om::vec3 direction = om::normalize(screenLocation - data->app->camera);

				Ray ray = Ray(data->app->camera, direction);
				int hit = 0;
#ifdef TIMEPERPIXEL
				GetPixelColor(ray, &data->app->bvh, &data->app->lightSource, 0, &rngState, -1, data->app->fastMode, hit);
				pixelColor += FColor((float)timer.GetCounter() * TIMEPERPIXEL, 1.f, 0).NormalHexColor();
#elif defined HEATMAP
				GetPixelColor(ray, &data->app->bvh, &data->app->lightSource, 0, &rngState, -1, data->app->fastMode, hit);
				pixelColor += FColor((float)hit, HEATMAP, 0).NormalHexColor();
#else
				pixelColor += GetPixelColor(ray, &data->app->bvh, &data->app->lightSource, 0, &rngState, -1, data->app->fastMode, hit);
#endif
				if (hit < 5)//if there's a single BVH node and thus a single triangle in front of the ray there's no use in anti aliasing it because every ray will return the same color.
				{
					currentAA = i + 1;
					break;
				}
			}
			lineStartColor[x] = (lineStartColor[x] * (float)data->app->antiAliasing + (pixelColor / (float)currentAA)) / ((float)data->app->antiAliasing + 1.0f);
			lineStart[x] = lineStartColor[x].HexColor();
		}
	}
	return 0;
}

void App::Tick(double deltaTime)
{
	CheckKeys((float)deltaTime);
#ifdef TIMING_COUNT
	timings[timingIndex] = deltaTime;
	timingIndex = (timingIndex + 1) % TIMING_COUNT;

	double avg = 0.0;
	for (int i = 0; i < TIMING_COUNT; ++i) {
		avg += timings[i];
	}
	avg *= 1.0 / TIMING_COUNT;

	printf("timing: %.03f (avg: %.03f)\n", deltaTime, avg);
#endif

	rotationMatrix = om::rotateYZ(rotationVector.x) * om::rotateXZ(rotationVector.y);

	om::vec3 planeDistance = om::vec3(0, 0, 1) * FOV;
	p0 = camera + rotationMatrix * (planeDistance + om::vec3(-1, 1, 0));
	p1 = camera + rotationMatrix * (planeDistance + om::vec3(1, 1, 0));
	p2 = camera + rotationMatrix * (planeDistance + om::vec3(-1, -1, 0));

	for (int i = 0; i < THREADCOUNT; i++)
	{
		thread[i] = SDL_CreateThread(RenderSection, "Ray tracing thread", &threadData[i]);
		if (thread[i] == NULL)
			printf("Failed creating a thread: %s\n", SDL_GetError());
	}
	for (int i = 0; i < THREADCOUNT; i++)
		SDL_WaitThread(thread[i], nullptr);

	antiAliasing++;
	frame++;
	engine->Update();
}

void App::CheckKeys(float deltaTime)
{
	float movementSpeed = 0.1f;
	if (keyDown[SDL_SCANCODE_LSHIFT])
		movementSpeed = 0.2f;
	if (keyDown[SDL_SCANCODE_LCTRL])
		movementSpeed = 0.01f;
	if (keyDown[SDL_SCANCODE_W])
		camera.z += movementSpeed * deltaTime;
	if (keyDown[SDL_SCANCODE_S])
		camera.z -= movementSpeed * deltaTime;
	if (keyDown[SDL_SCANCODE_A])
		camera.x -= movementSpeed * deltaTime;
	if (keyDown[SDL_SCANCODE_D])
		camera.x += movementSpeed * deltaTime;
	if (keyDown[SDL_SCANCODE_Q])
		camera.y -= movementSpeed * deltaTime;
	if (keyDown[SDL_SCANCODE_E])
		camera.y += movementSpeed * deltaTime;

	movementSpeed /= 10;

	if (keyDown[SDL_SCANCODE_UP])
		rotationVector.x += deltaTime * movementSpeed / PI;
	if (keyDown[SDL_SCANCODE_DOWN])
		rotationVector.x -= deltaTime * movementSpeed / PI;
	if (keyDown[SDL_SCANCODE_LEFT])
		rotationVector.y += deltaTime * movementSpeed / PI;
	if (keyDown[SDL_SCANCODE_RIGHT])
		rotationVector.y -= deltaTime * movementSpeed / PI;

	fastMode = false;
	for(int i = 4; i < 83; i++)//highest code that will make the camera move is 82 so no need to check the other keys.
		if (keyDown[i])
		{
			antiAliasing = 0;
			fastMode = true;
		}
}

void App::KeyDown(int key)
{
	if(key < 256)
		keyDown[key] = true;
}

void App::KeyUp(int key)
{
	if(key < 256)
		keyDown[key] = false;
	antiAliasing = 0;
}