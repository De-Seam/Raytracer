#include "precomp.h"
#include "engine.h"

#include "SDL_syswm.h"

Engine::Engine()
{
	pixels = new Pixel[SCRWIDTH * SCRHEIGHT];
	pitch = SCRWIDTH * 4; //apparently pitch is written to by sdl so this value doesn't matter.

	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Ray Tracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCRWIDTH, SCRHEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCRWIDTH, SCRHEIGHT);
	SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch);
}

void Engine::Update()
{
	SDL_UnlockTexture(buffer);
	//SDL_RenderCopyEx(renderer, buffer, NULL, NULL, NULL, NULL, SDL_RendererFlip(3)); //to flip it
	SDL_RenderCopy(renderer, buffer, NULL, NULL); // to not flip it

	SDL_RenderPresent(renderer);
	SDL_LockTexture(buffer, NULL, (void**)pixels, &pitch);
}

void Engine::ClearScreen(Pixel color)
{
	int temp = SCRWIDTH * SCRHEIGHT;
	for (int i = 0; i < temp; i++)
		pixels[i] = color;
}