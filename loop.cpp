#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include <cstdlib>
#include "defs.h"
#include "functions.h"
#include <thread>
using namespace std;
int StartTime;
int CurrentTime;
float TaskTime;
float FrameTime;
float TickTime;
float InputTime;
int LastTime;
float ProjectionTime;
// Camera Setup
float CameraX = 0;
float CameraY = 0;
float CameraZ = -10;
float CameraXVelocity = 0;
float CameraYVelocity = 0;
float CameraZVelocity = 0;
const int neighborOffsets[24][3] = {
	{-1,-1,-1},{ 0,-1,-1},{ 1,-1,-1},
	{-1, 0,-1},           { 1, 0,-1},
	{-1, 1,-1},{ 0, 1,-1},{ 1, 1,-1},
	{-1,-1, 0},{ 0,-1, 0},{ 1,-1, 0},
	{-1, 0, 0},           { 1, 0, 0},
	{-1, 1, 0},{ 0, 1, 0},{ 1, 1, 0},
	{-1,-1, 1},{ 0,-1, 1},{ 1,-1, 1},
	{-1, 0, 1},           { 1, 0, 1},
	{-1, 1, 1},{ 0, 1, 1},{ 1, 1, 1}
};
std::vector<thread> threads;
inline static void CellularAutomataRules(int txMin,int txMax,int tyMin, int tyMax) {
	// Cellular Automata do stuff now
	int survive; int neighbors;
	for (int i = txMin; i < txMax; i++) {
		for (int j = tyMin; j < tyMax; j++) {
			for (int k = 0; k < GameDepth; k++) {
				survive = 0; neighbors = 0;
				if (GameMap[i][j][k] == 1) { survive = 1; }
				// Determine Neighbors
				for (int o = 0; o < 24; o++) {
					int ni = i + neighborOffsets[o][0];
					int nj = j + neighborOffsets[o][1];
					int nk = k + neighborOffsets[o][2];
					if (ni >= 0 && ni < GameWidth && nj >= 0 && nj < GameHeight && nk >= 0 && nk < GameDepth) {
						if (GameMap[ni][nj][nk] == 1) {
							neighbors++;
						}
					}
				}
				if (neighbors < 2) { survive = 0; }// Underpopulation
				if (neighbors > 3) { survive = 0; }// Overpopulation
				if (neighbors == 3) { survive = 1; } // Reproduction
				if (survive == 1) {
					GameMapNext[i][j][k] = 1; // Cell survives
				}
				else {
					GameMapNext[i][j][k] = 0; // Cell dies
				}
			}
		}
	}
}



int game() {
	// Setting up the Timer
	int StartTime = SDL_GetTicks();
	int CurrentTime = SDL_GetTicks() - StartTime;
	int LastTime = CurrentTime;
	// Filling the Game Map with random values
	for (int i = 0; i < GameWidth; i++) {
		for (int j = 0; j < GameHeight; j++) {
			for (int k = 0; k < GameDepth; k++) {
				if (rand() % 10 > mapDensity * 10) {
					GameMap[i][j][k] = 1;
				}
				else {
					GameMap[i][j][k] = 0;
				}
			}
		}
	}
	// The Game Loop
	while (1) {
		CurrentTime = SDL_GetTicks() - StartTime;
		FrameTime = SDL_GetTicks();
		if (CurrentTime - LastTime >= TickInterval && Pause == 0) {
			if (Debug == true) { TickTime = SDL_GetTicks(); }
			LastTime = CurrentTime;
			// emptying GameMapNext
			for (auto& row : GameMapNext) {
				for (auto& depthSlice : row) {
					std::fill(depthSlice.begin(), depthSlice.end(), 0);
				}
			}
			// Cellular Automata Logic
			threads.clear();
			int rowLength = GameHeight / ThreadCountUsed;
			for (int i = 0; i < ThreadCountUsed; i++) {
				int yMin = i * rowLength;
				int yMax = (i == ThreadCountUsed - 1) ? GameHeight : (i + 1) * rowLength; // the last thread takes the remaining rows
				threads.emplace_back(CellularAutomataRules, 0, GameWidth, yMin, yMax);
			}
			for (auto& th : threads) { th.join(); };
			// Apply Changes
			std::swap(GameMap, GameMapNext); // Basically GameMap = GameMapNext; but Copilot says it's faster lol
			if (Debug == true) { TickTime = SDL_GetTicks() - TickTime; }
		}
		// User Input
		if (Debug == true) { InputTime = SDL_GetTicks(); }
		inputHandler();
		if (Debug == true) { InputTime = SDL_GetTicks() - InputTime; }
		// rendering
		if (Debug == true) { RenderTime = SDL_GetTicks(); }
		render(GameMap);
		if (Debug == true) { RenderTime = SDL_GetTicks() - RenderTime; }
		FrameTime = SDL_GetTicks() - FrameTime; // Frametime measurement end
		// Debug Message
		if (Debug == true) { cout << "----------------------------------------" << endl; }
		if (Debug == true) { cout << "FPS: " << 1000.0f / FrameTime << "   FrameTime: " << FrameTime << "ms" << "   Rendering: " << RenderTime << "ms" << "   Rectangles: " << RenderRectangleTime << "ms" << "   RenderInit: " << RenderInitTime << "ms" << "   RenderPresent: " << RenderPresentTime << "ms" << endl; };
		if (Debug == true) { cout << "TickTime: " << TickTime << "ms" << "   ReadVoxels: " << ReadVoxelTime << "ms" << "   DepthSorting: " << DepthSortTime << "ms" << "   3D to 2D Projection: " << ProjectionTime << "ms" << "   InputTime: " << InputTime << "ms" << endl; }
		// Cleanup
		if (SDL_PollEvent(&event) && event.type == SDL_EVENT_QUIT) { break; }
	}
	return 0;
}