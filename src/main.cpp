
#include <iostream>
#include <cassert>

#include <SDL.h>
#include <gl/glew.h>
#include <SDL_opengl.h>

#include "Window/Window.hpp"

int main(int argc, char **argv)
{
	Window window;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(1);

	window.Load("Testy", 800, 600);
	window.Bind();

	bool Quit = false;
	SDL_Event Event;
	while (!Quit)
	{
		while (SDL_PollEvent(&Event))
		{
			switch (Event.type)
			{

			case SDL_QUIT:
				Quit = true;
			}
		}
		glClearColor(1, 0.5, 0.5, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(window);
	}

	return 0;
}