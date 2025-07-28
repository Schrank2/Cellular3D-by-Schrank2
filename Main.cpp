#include <iostream>
#include <SDL3/SDL.h>
#include "functions.h"
#include "defs.h"
#include <thread>
#include <vector>
#include <string>
#include <functional>
using namespace std;
int GameScale;
float mapDensity;
int ScreenWidth;
int ScreenHeight;
float ThreadCountUsage;
int TickTime;
bool Debug;
int GameWidth;
int GameHeight;
int GameDepth = 3; // The depth of the game map, can be used for 3D rendering
int ThreadCount;
int ThreadCountUsed; // Number of threads used
SDL_Window* window;
SDL_Renderer* renderer;
vector<vector<vector<int>>> GameMap;
vector<vector<vector<int>>> GameMapNext;
// using pointers because weird
int main(int argc, char* argv[])
{
	// Declaring Globals based on the settings
	vector<string> settings = readSettings();
	GameScale = stoi(settings[0]); // The Dimensions of each tile in pixels
	mapDensity = stof(settings[1]); // The density of Cells in the beginning in 0.0 -> 1.0
	ScreenWidth = stoi(settings[2]); // The width of the game window in pixels
	ScreenHeight = stoi(settings[3]); // The height of the game window in pixels
	ThreadCountUsage = stof(settings[4]); // The percentage of CPU threads to use for the game
	TickTime = stoi(settings[5]); // Time between game ticks in milliseconds
	if (settings[6] == "true") { Debug = true; }; // Should debug messages be printed to the console
	GameWidth = ScreenWidth / GameScale;
	GameHeight = ScreenHeight / GameScale;
	ThreadCount = thread::hardware_concurrency();
	ThreadCountUsed; // Number of threads used
	ThreadCountUsed = ThreadCount * ThreadCountUsage;
	GameMap.resize(GameWidth, vector<vector<int>>(GameHeight, vector<int>(GameDepth)));
	GameMapNext.resize(GameWidth, vector<vector<int>>(GameHeight, vector<int>(GameDepth)));
	if (Debug = true) { cout << "Successfully Read Settings" << endl; }
	if (SDL_Init( SDL_INIT_VIDEO or SDL_INIT_AUDIO) < 0)
	{
		cout << "SDL initialization failed. SDL Error: " << SDL_GetError();
	}
	else
	{
		cout << "SDL initialization succeeded! \n";
	}
	window = SDL_CreateWindow("CELLULAR AUTOMATA", ScreenWidth, ScreenHeight, SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, nullptr);;
	cout << "Available Threads: " << ThreadCount << "   Using " << ThreadCountUsed << " Threads" << endl;
	game();
	cin.get();
	return 0;
}

