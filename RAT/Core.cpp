#include "Core.h"
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <stdio.h>
#include <iostream>

#define SUCCESS 0

Core::Core()
{
  if (!appInitialization())
  {
    printf("App Initialization not successful\n");
  }
}

Core::~Core()
{
		IMG_Quit();
		if (TTF_WasInit()) 
		{
				TTF_Quit();
		}

		if (mainRenderer != NULL)
		{
				SDL_DestroyRenderer(mainRenderer);
				mainRenderer = NULL;
		}

		if (mainWindow != NULL)
		{
				SDL_DestroyWindow(mainWindow);
				mainWindow = NULL;
		}
		SDL_Quit();
}

SDL_Renderer* Core::getRenderer()
{
  return mainRenderer;
}

SDL_Window* Core::getWindow()
{
  return mainWindow;
}

void Core::setWindowTitle(const std::string& title)
{
		if (mainWindow)
		{
				SDL_SetWindowTitle(mainWindow, title.c_str());
		}
}

float Core::getTargetDeltaTime()
{
		return targetDeltaTime;
}

Uint64 Core::getTargetFPS()
{
		return targetFPS;
}

bool Core::appInitialization()
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

  if (SDL_GetNumVideoDisplays() <= 0)
  {
    printf("No display monitors attached to current device: %s\n", SDL_GetError());
    return false;
  }

		mainWindow = SDL_CreateWindow("Richie's Advanced Tactics", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (mainWindow == NULL)
		{
				printf("Main window creation failed : %s\n", SDL_GetError());
				return false;
		}

		mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (mainRenderer == NULL)
		{
				printf("Main renderer creation failed : %s\n", SDL_GetError());
				return false;
		}

		if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP) == 0)
		{
				printf("Failed to initialize SDL IMG Library: %s\n", IMG_GetError());
				return false;
		}

		if (TTF_Init() == -1)
		{
				printf("Failed to initialize SDL TTF Library: %s\n", TTF_GetError());
				return false;
		}

		SDL_DisplayMode displayMode;
		//get the first monitor attached to device display mode settings.
		if (SDL_GetDisplayMode(0, 0, &displayMode) != SUCCESS)
		{
				printf("Unable to get display mode settings : %s\n", SDL_GetError());
				return false;
		}

		//cap targetFPS to 60
		targetFPS = displayMode.refresh_rate > 60 ? 60 : displayMode.refresh_rate;
		targetDeltaTime = 1000.0f / targetFPS;

  return true;
}
