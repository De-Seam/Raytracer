#pragma once
class Engine
{
public:
	Pixel* pixels;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* buffer;
	int pitch;
	Engine();
	void ClearScreen(Pixel color);
	void Update();
};