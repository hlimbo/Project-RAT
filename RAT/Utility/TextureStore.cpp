#include "TextureStore.h"
#include <SDL2/SDL_render.h>
#include <SDL2_image/SDL_image.h>

TextureStore::TextureStore(SDL_Renderer* renderer) : renderer(renderer) {}

TextureStore::~TextureStore()
{
  int destroyCount = 0;
  for (auto it = textures.begin(); it != textures.end();)
  {
    SDL_Texture* texture = it->second;
    it = textures.erase(it);
    destroyCount = Free(texture) ? destroyCount + 1 : destroyCount;
  }

  if (renderer)
  {
    renderer = nullptr;
  }

  printf("destroy count: %d\n", destroyCount);
}

SDL_Texture* TextureStore::Load(const std::string& fileName, const std::string& filePath)
{
  if (textures.find(fileName) != textures.end())
  {
    return NULL;
  }

  SDL_Texture* texture = Load(filePath.c_str());
  if (texture == NULL)
  {
    return NULL;
  }

  textures.insert(make_pair(fileName, texture));
  return texture;
}

SDL_Texture* TextureStore::Get(const std::string& fileName)
{
  return textures.at(fileName);
}

bool TextureStore::Free(const std::string& fileName)
{
  SDL_Texture* texture = Get(fileName);
  textures.erase(fileName);
  return Free(texture);
}

SDL_Texture* TextureStore::Load(const char* filePath)
{
  if (renderer == NULL)
  {
    fprintf(stderr, "TextureStore renderer not found\n");
    return NULL;
  }

  SDL_Texture* texture = IMG_LoadTexture(renderer, filePath);
  if (texture == NULL)
  {
    fprintf(stderr, "%s could not be loaded: %s\n", filePath, IMG_GetError());
  }

  return texture;
}

bool TextureStore::Free(SDL_Texture* texture)
{
  if (texture == NULL)
  {
    return false;
  }

  SDL_DestroyTexture(texture);
  texture = NULL;

  return true;
}
