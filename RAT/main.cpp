#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#include "Core.h"
#include "Utility/TextureStore.h"

int main(int argc, char* argv[])
{
  std::string mainPath(SDL_GetBasePath());
  mainPath += std::string("resources\\");
  printf("asset path: %s\n", mainPath.c_str());

  std::string playerPath = mainPath + std::string("marche.png");
  std::string tilesPath = mainPath + std::string("isometric_tiles.png");

  Core core;
  TextureStore store(core.getRenderer());
  SDL_Texture* playerTexture = store.Load("marche.png", playerPath);

  int srcWidth, srcHeight;
  if (SDL_QueryTexture(playerTexture, NULL, NULL, &srcWidth, &srcHeight) != 0)
  {
    fprintf(stderr, "%s\n", SDL_GetError());
    return -1;
  }

  const SDL_Rect playerBounds{ 0, 0, srcWidth * 2, srcHeight * 2 };


  // Game Loop

  Uint64 performanceFrequency = SDL_GetPerformanceFrequency();
  Uint64 startCount = SDL_GetPerformanceCounter();
  float targetDeltaTime = core.getTargetDeltaTime();
  float currentTime = 0;
  float deltaTime;

  bool running = true;
  while (running)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        running = false;
      }
    }

    // white background
    SDL_SetRenderDrawColor(core.getRenderer(), 255, 255, 255, 255);

    int status = SDL_RenderCopy(core.getRenderer(), playerTexture, NULL, &playerBounds);
    if (status != 0)
    {
      fprintf(stderr, "render error: %s\n", SDL_GetError());
    }

    SDL_RenderPresent(core.getRenderer());
    SDL_RenderClear(core.getRenderer());


    Uint64 endCount = SDL_GetPerformanceCounter();
    float observedDeltaTime = (1000.0f * (endCount - startCount) / performanceFrequency);
    Uint64 observedFPS = performanceFrequency / (endCount - startCount);

    float msDifference = targetDeltaTime - observedDeltaTime;
    if (msDifference > 0)
    {
      SDL_Delay((Uint32)msDifference);
      endCount = SDL_GetPerformanceCounter();
      observedDeltaTime = (1000.0f * (endCount - startCount) / performanceFrequency);
      observedFPS = performanceFrequency / (endCount - startCount);
    }

    currentTime += observedDeltaTime;
    deltaTime = observedDeltaTime / 1000.0f;
    startCount = endCount;

    if ((int)currentTime % 100 == 0)
    {
      std::string title("Richie's Advanced Tactics | FPS: ");
      title += std::to_string(observedFPS);
      core.setWindowTitle(title);
    }
  }



  return 0;
}