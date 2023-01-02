#ifndef TEXTURE_STORE_H
#define TEXTURE_STORE_H
#include <unordered_map>
#include <string>


struct SDL_Texture;
struct SDL_Renderer;

class TextureStore
{
public:
  TextureStore(SDL_Renderer* renderer);
  ~TextureStore();

  SDL_Texture* Load(const std::string& fileName, const std::string& filePath);
  bool Free(const std::string& fileName);
  SDL_Texture* Get(const std::string& fileName);

private:
  SDL_Texture* Load(const char* filePath);
  bool Free(SDL_Texture* texture);

  SDL_Renderer* renderer;
  std::unordered_map<std::string, SDL_Texture*> textures;
};

#endif

