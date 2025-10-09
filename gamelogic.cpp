#include <iostream>
#include <SDL3/SDL.h>
#include "functions.h"
#include "defs.h"
#include <thread>
#include <vector>
#include <string>
using namespace std;

// Input Setup
float mouseX, mouseY;
int mouseXgame, mouseYgame;
KEYBOARD Keyboard = KEYBOARD(false, false, false, false, false, false);

void inputHandler() {
	SDL_GetMouseState(&mouseX, &mouseY); // Check mouse position
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
		if (event.key.key == SDLK_P) { Pause = !Pause; }
		if (event.key.key == SDLK_W) { Keyboard.W = true; }
		if (event.key.key == SDLK_A) { Keyboard.A = true; }
		if (event.key.key == SDLK_S) { Keyboard.S = true; }
		if (event.key.key == SDLK_D) { Keyboard.D = true; }
		if (event.key.key == SDLK_SPACE) { Keyboard.SPACE = true; }
		if (event.key.key == SDLK_LSHIFT) { Keyboard.LSHIFT = true; }
	}
	if (event.type == SDL_EVENT_KEY_UP) {
		if (event.key.key == SDLK_W) { Keyboard.W = false; }
		if (event.key.key == SDLK_A) { Keyboard.A = false; }
		if (event.key.key == SDLK_S) { Keyboard.S = false; }
		if (event.key.key == SDLK_D) { Keyboard.D = false; }
		if (event.key.key == SDLK_SPACE) { Keyboard.SPACE = false; }
		if (event.key.key == SDLK_LSHIFT) { Keyboard.LSHIFT = false; }
	}
	if (Keyboard.W == true){ CameraZVelocity += 0.1; }
	if (Keyboard.A == true) { CameraXVelocity += 0.1; }
	if (Keyboard.S == true) { CameraZVelocity -= 0.1; }
	if (Keyboard.D == true) { CameraXVelocity -= 0.1; }
	if (Keyboard.SPACE == true) { CameraYVelocity -= 0.1; }
	if (Keyboard.LSHIFT == true) { CameraYVelocity += 0.1; }
	// Moving the Camera according to its velocity
	CameraX += CameraXVelocity * 0.3;
	CameraXVelocity *= 0.9;
	CameraY += CameraYVelocity * 0.3;
	CameraYVelocity *= 0.9;
	CameraZ += CameraZVelocity * 0.3;
	CameraZVelocity *= 0.9;
	if (Debug == true) { InputTime = SDL_GetTicks() - InputTime; }
	FrameTime = SDL_GetTicks() - FrameTime;
	if (Debug == true) { cout << "----------------------------------------" << endl; }
	if (Debug == true) { cout << "FPS: " << 1000.0f / FrameTime << "   FrameTime: " << FrameTime << "ms" << "   Rendering: " << RenderTime << "ms" << "   Voxels: " << RenderVoxelTime << "ms" << "   Rectangles: " << RenderRectangleTime << "ms" << "   RenderInit: " << RenderInitTime << "ms" << "   RenderPresent: " << RenderPresentTime << "ms" << endl; };
	if (Debug == true) { cout << "TickTime: " << TickTime << "ms" << "   ReadVoxels: " << ReadVoxelTime << "ms" << "   DepthSorting: " << DepthSortTime << "ms" << "   Drawing: " << DrawTime << "ms" << "   InputTime: " << InputTime << "ms" << endl; }
}