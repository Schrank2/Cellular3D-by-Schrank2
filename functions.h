#include <vector>
#include <SDL3/SDL.h>
#include <string>
#include <functional>
std::vector<std::string> readSettings();
int main(int argc, char* argv[]);
int game();
void render(const std::vector<std::vector<int>>& GameMap);
SDL_Texture* genCellTexture();
void executeThreaded(std::function<void(int)> func);

