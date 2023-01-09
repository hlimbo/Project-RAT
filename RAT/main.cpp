#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#include "Core.h"
#include "Utility/TextureStore.h"

#define SCALE 2

struct Tile
{
  SDL_Rect rect;
  int layer;
};


SDL_Point isoToScreenCoords(int x, int y, int tileWidth, int tileHeight)
{
  return SDL_Point
  {
    (x - y) * (tileWidth / 2), // screen x
    (x + y) * (tileHeight / 2) // screen y
  };
}

// Need to verify -- rounds down the floating point calculations as these are in pixel units
SDL_Point screenToIsoCoords(int x, int y, int tileWidth, int tileHeight, int originX, int originY)
{
  return SDL_Point
  {
    (int)(((float)1 / tileWidth) * (x - originX) + ((float)1 / tileHeight) * (y - originY)),
    (int)(((float)1 / tileHeight) * (y - originY) - ((float)1 / tileWidth) * (x - originX))
  };
}

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
  SDL_Texture* tileTextures = store.Load("isometric_tiles.png", tilesPath);


  int srcPlayerWidth, srcPlayerHeight;
  if (SDL_QueryTexture(playerTexture, NULL, NULL, &srcPlayerWidth, &srcPlayerHeight) != 0)
  {
    fprintf(stderr, "%s\n", SDL_GetError());
    return -1;
  }

  int srcWidth, srcHeight;
  if (SDL_QueryTexture(tileTextures, NULL, NULL, &srcWidth, &srcHeight) != 0)
  {
    fprintf(stderr, "%s\n", SDL_GetError());
    return -1;
  }

  // 3 tiles in sprite-sheet
  int tileWidth = (srcWidth / 3);
  int tileHeight = srcHeight;
  const SDL_Rect tile2Bounds{ 64, 0, tileWidth, tileHeight };
  const SDL_Rect tile2Offset{ 0, 0, tileWidth * SCALE, tileHeight * SCALE };

  SDL_Point offsetOrigin{ (SCREEN_WIDTH / 2) - (tileWidth * SCALE / 2), 0 };
  const SDL_Rect playerBounds{ (SCREEN_WIDTH / 2) - (srcPlayerWidth * SCALE / 2), -8, srcPlayerWidth * SCALE, srcPlayerHeight * SCALE};

  // bounds relative to spritesheet texture loaded
  const SDL_Rect tile1Bounds{ 0, 0, tileWidth, tileHeight };

  SDL_Rect spriteTileBounds[3];
  spriteTileBounds[0] = SDL_Rect{ 0, 0, tileWidth, tileHeight };
  spriteTileBounds[1] = SDL_Rect{ 64, 0, tileWidth, tileHeight };
  // highlight tile sprite
  spriteTileBounds[2] = SDL_Rect{ 32, 0, tileWidth, tileHeight };

  // Multilevel 392 both floors completely filled
  // 294 = first floor completely filled | second floor filled half way
  const int MAX_LAYERS = 2;
  const int GRID_SIZE = 14; // GRID_SIZE * GRID_SIZE + (GRID_SIZE * GRID_SIZE / 2)
  // disadvantage of this structure is that we don't have O(1) lookup of the layer whereas 2d array would where first index represents layer index and second index represents tile index
  // alternative Tile screenTiles[MAX_LAYERS][GRID_SIZE * GRID_SIZE];
  // GRID_SIZE * GRID_SIZE + (MAX_LAYERS * GRID_SIZE * GRID_SIZE) / 2;
  Tile screenTiles[MAX_LAYERS * GRID_SIZE * GRID_SIZE];
  for (int layer = 0; layer < MAX_LAYERS; ++layer) {
    Tile tile;
    tile.layer = layer;
    for (int y = 0; y < GRID_SIZE; ++y) {
      for (int x = 0; x < GRID_SIZE; ++x) {
        SDL_Point screenPosition = isoToScreenCoords(x, y, tileWidth * SCALE, tileHeight * SCALE);
        screenPosition.x += offsetOrigin.x;
        screenPosition.y += offsetOrigin.y;
        tile.rect = SDL_Rect{ screenPosition.x, screenPosition.y, tileWidth * SCALE, tileHeight * SCALE };
        screenTiles[GRID_SIZE * GRID_SIZE * layer + GRID_SIZE * y + x] = tile;
      }
    }
  }

  int mouseX = 0, mouseY = 0;
  SDL_Point mouseIsoPos{ mouseX, mouseY };

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
      else if (event.type == SDL_MOUSEMOTION)
      {
        mouseX = event.motion.x;
        mouseY = event.motion.y;
        mouseIsoPos = screenToIsoCoords(mouseX, mouseY, tileWidth * SCALE, tileHeight * SCALE, offsetOrigin.x, offsetOrigin.y);

      }
    }

    // white background
    SDL_SetRenderDrawColor(core.getRenderer(), 255, 255, 255, 255);

    // ISO TILES
    for (int i = 0; i < MAX_LAYERS * GRID_SIZE * GRID_SIZE; ++i) 
    {
      //if (i == 0 || i == 16) continue;
      int layer = screenTiles[i].layer;
      // hardcode stop rendering tiles on the second floor... only display half computed on 2nd floor
      if (layer == 1 && i >= 294) {
        break;
      }
      SDL_RenderCopy(core.getRenderer(), tileTextures, &spriteTileBounds[layer], &screenTiles[i].rect);
    }

    // draw tiles backwards? then the tiles get drawn in a pyramid like structure
    //for (int i = GRID_SIZE * GRID_SIZE - 1; i > GRID_SIZE * GRID_SIZE - GRID_SIZE; --i)
    //{
    //  // if (i == GRID_SIZE * GRID_SIZE - 15) continue;
    //  SDL_RenderCopy(core.getRenderer(), tileTextures, &tile1Bounds, &tileScreenRects[i]);
    //}

    // render tile highlighter as long as it is within grid bounds
    if (mouseIsoPos.x >= 0 && mouseIsoPos.x < GRID_SIZE && mouseIsoPos.y >= 0 && mouseIsoPos.y < GRID_SIZE)
    {
      SDL_Rect* tileHighlighter = &spriteTileBounds[2];
      SDL_Point screenPos = isoToScreenCoords(mouseIsoPos.x, mouseIsoPos.y, tileWidth * SCALE, tileHeight * SCALE);
      screenPos.x += offsetOrigin.x;
      screenPos.y += offsetOrigin.y;

      SDL_Rect highlightRect = SDL_Rect{ screenPos.x, screenPos.y, tileWidth * SCALE, tileHeight * SCALE };
      SDL_RenderCopy(core.getRenderer(), tileTextures, tileHighlighter, &highlightRect);
    }

    // player
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

    if ((int)currentTime % 1000 == 0)
    {
      std::string title("Richie's Advanced Tactics | FPS: ");
      title += std::to_string(observedFPS);
      core.setWindowTitle(title);
    }
  }



  return 0;
}