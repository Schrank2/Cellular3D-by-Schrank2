#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include <cstdlib>
#include "defs.h"
#include "functions.h"
#include <thread>
using namespace std;
SDL_Texture* cellTexture;
int StartTime;
int CurrentTime;
int LastTime;
int CameraX = 0;
int CameraY = 0;
int CameraZ = 0;
int GameTemp = 0;
int Pause = 0;
int k = 1;
float mouseX, mouseY;
int mouseXgame, mouseYgame;
const int neighborOffsets[8][2] = {
	{-1, -1}, {0, -1}, {1, -1},
	{-1,  0},         {1,  0},
	{-1,  1}, {0,  1}, {1,  1}
};
std::vector<thread> threads;
static void CellularAutomataRules(int txMin,int txMax,int tyMin, int tyMax) {
	// Cellular Automata do stuff now
	int survive; int neighbors;
	for (int i = txMin; i < txMax; i++) {
		for (int j = tyMin; j < tyMax; j++) {
			for (int k = tyMin; k < tyMax; k++) {
				survive = 0; neighbors = 0;
				if (GameMap[i][j][k] == 1) { survive = 1; }
				// Determine Neighbors
				for (int o = 0; o < 8; o++) {
					int ni = i + neighborOffsets[o][0];
					int nj = j + neighborOffsets[o][1];
					if (ni >= 0 && ni < GameWidth && nj >= 0 && nj < GameHeight) {
						if (GameMap[ni][nj][k] == 1) {
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
	SDL_Event event;
	// Setting up the Timer
	int StartTime = SDL_GetTicks();
	int CurrentTime = SDL_GetTicks() - StartTime;
	int LastTime = CurrentTime;
	// Filling the Game Map with random values
	for (int i = 0; i < GameWidth; i++) {
		for (int j = 0; j < GameHeight; j++) {
			for (int k = 0; k < GameHeight; k++) {
				if (rand() % 10 > mapDensity * 10) {
					GameMap[i][j][k] = 1;
				}
				else {
					GameMap[i][j][k] = 0;
				}
			}
		}
	}
	// Generating the Textures
	cellTexture=genCellTexture();
	// The Game Loop
	while (1) {
		CurrentTime = SDL_GetTicks() - StartTime;
		if (CurrentTime - LastTime >= TickTime && Pause == 0) {
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
		}
		render(GameMap);
		SDL_GetMouseState(&mouseX, &mouseY); // Check mouse position
		if (SDL_PollEvent(&event) && event.type == SDL_EVENT_QUIT)
			break;
		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				mouseXgame = mouseX / GameScale;
				mouseYgame = mouseY / GameScale;
				if (mouseXgame >= 0 && mouseXgame < GameWidth && mouseYgame >= 0 && mouseYgame < GameHeight) {
					GameMap[mouseXgame][mouseYgame][0] = !GameMap[mouseXgame][mouseYgame][0];
				}
			}
		}
		if (event.type == SDL_EVENT_KEY_DOWN) {
			if (event.key.key == SDLK_SPACE) {
				Pause = !Pause;
			}
		}
	}
	// Cleanup
	return 0;
}