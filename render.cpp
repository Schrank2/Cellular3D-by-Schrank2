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
mutex renderLock;
vector<thread> RenderThreads;
vector<vector<SDL_FRect>> RenderRects(ThreadCountUsed);

SDL_Texture* genCellTexture() { // Lots of Help from Copilot
	// Setup the Texture
	SDL_Texture* Texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_TARGET,
		GameScale,
		GameScale
	);
	// Check if successful
	if (!Texture) {
		std::cerr << "Failed to create cell texture: " << SDL_GetError() << std::endl;
		return nullptr;
	}
	// Save current render target
	SDL_Texture* prevTarget = SDL_GetRenderTarget(renderer);
	// Set Texture as render target
	SDL_SetRenderTarget(renderer, Texture);
	// Clear the texture
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	// Draw the texture
	for (int l = 0; l < GameScale; l++) {
		for (int m = 0; m < GameScale; m++) {
			temp = 100 + (40.0f * m / GameScale) + (40.0f * l / GameScale);
			SDL_SetRenderDrawColor(renderer, 0, temp, 0, 255);
			SDL_RenderPoint(renderer, l, m);
		}
	}
	// Reset render target
	SDL_SetRenderTarget(renderer, prevTarget);
	return Texture;
}

static void renderThreaded(int Thread,const std::vector<std::vector<std::vector<int>>>& GameMap,int txMin, int txMax, int tyMin, int tyMax) {
	SDL_FRect rect;
	for (int i = txMin; i < txMax; i++) {
		for (int j = tyMin; j < tyMax; j++) {
			if (GameMap[i][j][1] == 1) {
				// Drawing the Texture onto the screen
				rect = { static_cast<float>(i * GameScale), static_cast<float>(j * GameScale), static_cast<float>(GameScale), static_cast<float>(GameScale)};
				renderLock.lock(); // Used to avoid Deadlock Issue
				RenderRects[Thread].push_back(rect);
				renderLock.unlock();
			}
		}
	}
}

void render(const vector<vector<vector<int>>>& GameMap) {
	if (!cellTexture) { // Check if cellTexture is there
		std::cerr << "cellTexture is null!" << std::endl;
		return;
	}
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer); // Clear the screen with white color
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	// Rendering Multithreaded
	render3D();
	// Rendering Multithreaded
	RenderThreads.clear();
	for (auto& rects : RenderRects) { // clear old rectangles
		rects.clear();
	}
	RenderRects.resize(ThreadCountUsed);
	int rowLength = GameHeight / ThreadCountUsed;
	for (int i = 0; i < ThreadCountUsed; i++) {
		int yMin = i * rowLength;
		int yMax = (i == ThreadCountUsed - 1) ? GameHeight : (i + 1) * rowLength; // the last thread takes the remaining rows
		RenderThreads.emplace_back(renderThreaded, i, GameMap, 0, GameWidth, yMin, yMax);
	}
	for (auto& th : RenderThreads) { th.join(); }; // Wait for the Rectangles to be calculated
	for (int j = 0; j < ThreadCountUsed; j++) { // Draw the Textures into the Rectangles
		for (int i = 0; i < RenderRects[j].size(); i++) {
			SDL_RenderTexture(renderer, cellTexture, nullptr, &RenderRects[j][i]);
		}
	}
	// Showing the Result
	SDL_RenderPresent(renderer);
}
