#include <SDL3/SDL.h>
#include <vector>
#ifndef COMMON_H
#define COMMON_H
extern int CurrentTime;
extern int StartTime;
extern int LastTime;
extern int Toggle;
extern int CameraX;
extern int CameraY;
extern int CameraZ;
extern int GameWidth;
extern int GameHeight;
extern int GameDepth;
extern int ThreadCount;
extern int ThreadCountUsed;
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Event event;
extern SDL_Texture* cellTexture;
extern std::vector<std::vector<std::vector<int>>> GameMap;
extern std::vector<std::vector<std::vector<int>>> GameMapNext;
// Settings
extern int GameScale; // The Dimensions of each tile in pixels
extern float mapDensity; // The density of Cells in the beginning in 0.0 -> 1.0
extern int ScreenWidth; // The width of the game window in pixels
extern int ScreenHeight; // The height of the game window in pixels
extern float ThreadCountUsage; // The percentage of CPU threads to use for the game
extern int TickTime; // Time between game ticks in milliseconds
extern bool Debug; // Should debug messages be printed to the console
#endif