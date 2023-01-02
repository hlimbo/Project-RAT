#ifndef CORE_H
#define CORE_H

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 620

#include <SDL2/SDL_stdinc.h>
#include <string>

struct SDL_Window;
struct SDL_Renderer;


class Core
{
public:
  Core();
  ~Core();

  SDL_Renderer* getRenderer();
  SDL_Window* getWindow();
  void setWindowTitle(const std::string& title);
  float getTargetDeltaTime();
  Uint64 getTargetFPS();

private:
  bool appInitialization();

  SDL_Window* mainWindow;
  SDL_Renderer* mainRenderer;
  Uint64 targetFPS;
  float targetDeltaTime;
};

#endif

