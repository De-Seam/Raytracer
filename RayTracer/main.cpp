#include "precomp.h"

int main()
{
	double lastTime = 0, currentTime;
	bool exitApp = false;
	Engine* engine = new Engine;
	App* app = new App;
	/*
	{
		int mouseX, mouseY;
		SDL_GetGlobalMouseState(&mouseX, &mouseY);
		om::srand(mouseX * mouseY);
	}*/
	Timer timer;
	timer.StartCounter();
	app->Init(engine);
	currentTime = timer.GetCounter();
	while (!exitApp)
	{
		app->Tick(currentTime - lastTime);
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				exitApp = true;
				break;
			case SDL_KEYDOWN:
				app->KeyDown(event.key.keysym.scancode);
				break;
			case SDL_KEYUP:
				app->KeyUp(event.key.keysym.scancode);
				break;
			default:
				break;
			}
		}
		lastTime = currentTime;
		currentTime = timer.GetCounter();
	}

	SDL_Quit();

	return 0;
}