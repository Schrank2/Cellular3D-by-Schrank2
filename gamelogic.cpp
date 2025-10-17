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
KEYBOARD Keyboard = KEYBOARD(false, false, false, false, false, false, false, false);

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
		// Toggle Pause
		if (event.key.key == SDLK_P) { Pause = !Pause; }
		// Camera Movement
		if (event.key.key == SDLK_W) { Keyboard.W = true; }
		if (event.key.key == SDLK_A) { Keyboard.A = true; }
		if (event.key.key == SDLK_S) { Keyboard.S = true; }
		if (event.key.key == SDLK_D) { Keyboard.D = true; }
		if (event.key.key == SDLK_SPACE) { Keyboard.SPACE = true; }
		if (event.key.key == SDLK_LSHIFT) { Keyboard.LSHIFT = true; }
		// Camera Rotation
		if (event.key.key == SDLK_C) { Keyboard.C = true; }
		if (event.key.key == SDLK_V) { Keyboard.V = true; }
	}
	if (event.type == SDL_EVENT_KEY_UP) {
		// Camera Movement
		if (event.key.key == SDLK_W) { Keyboard.W = false; }
		if (event.key.key == SDLK_A) { Keyboard.A = false; }
		if (event.key.key == SDLK_S) { Keyboard.S = false; }
		if (event.key.key == SDLK_D) { Keyboard.D = false; }
		if (event.key.key == SDLK_SPACE) { Keyboard.SPACE = false; }
		if (event.key.key == SDLK_LSHIFT) { Keyboard.LSHIFT = false; }
		// Camera Rotation
		if (event.key.key == SDLK_C) { Keyboard.C = false; }
		if (event.key.key == SDLK_V) { Keyboard.V = false; }
	}
	// Apply held key effects
	if (Keyboard.W == true){ C1.vz += 0.1; }
	if (Keyboard.A == true) { C1.vx += 0.1; }
	if (Keyboard.S == true) { C1.vz -= 0.1; }
	if (Keyboard.D == true) { C1.vx -= 0.1; }
	if (Keyboard.SPACE == true) { C1.yaw -= 1.0; }
	if (Keyboard.LSHIFT == true) { C1.pitch += 1.0; }
	// Moving the Camera according to its velocity
	C1.x += C1.vx * 0.3;
	C1.vx *= 0.9;
	C1.y += C1.vy * 0.3;
	C1.vy *= 0.9;
	C1.z += C1.vz * 0.3;
	C1.vz *= 0.9;
}