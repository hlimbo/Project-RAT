#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#include "Core.h"

int main(int argc, char* argv[])
{
  std::cout << "hello world" << std::endl;
  Core core;


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