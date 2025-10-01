#include <iostream>
#include <SDL3/SDL.h>
#include <vector>
#include <thread>
#include "defs.h"
#include "functions.h"
#include <mutex>
using namespace std;
int Toggle=0;
float temp = 0;
float RenderTime = 0.0f;
float RenderVoxelTime;
float RenderRectangleTime;
float RenderInitTime = 0.0f;
float RenderPresentTime = 0.0f;
float ReadVoxelTime;
float DepthSortTime;
float DrawTime;
vector<vector<SDL_FRect>> RenderRects(ThreadCountUsed);

void render(const vector<vector<vector<int>>>& GameMap) {
	if (Debug == true) { RenderInitTime = SDL_GetTicks(); }
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer); // Clear the screen with white color
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	if (Debug == true) { RenderInitTime = SDL_GetTicks() - RenderInitTime; }
	// Rendering 3D
	render3D();
	// Showing the Result
	if (Debug == true) { RenderPresentTime = SDL_GetTicks(); }
	SDL_RenderPresent(renderer);
	if (Debug == true) { RenderPresentTime = SDL_GetTicks() - RenderPresentTime; }
}
