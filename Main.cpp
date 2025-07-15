#include <iostream>
#include <SDL3/SDL.h>
#include "functions.h"
#include "defs.h"
#include <thread>
#include <vector>
#include <string>
#include <functional>
using namespace std;
std::vector<string> settings;
int GameScale;
float mapDensity;
int ScreenWidth;
int ScreenHeight;
float ThreadCountUsage;
int TickTime;
int GameWidth;
int GameHeight;
int ThreadCount;
int ThreadCountUsed; // Number of threads used
SDL_Window* window;
SDL_Renderer* renderer;
vector<vector<int>> GameMap;
vector<vector<int>> GameMapNext;
vector<thread> ThreadList;
// using pointers because weird
void executeThreaded(std::function<void(int)> func) {
	int rowLength = GameHeight / ThreadCountUsed;
	for (int i = 0; i < ThreadCountUsed; i++) {
		int yMin = i * rowLength;
		int yMax = (i == ThreadCountUsed - 1) ? GameHeight : (i + 1) * rowLength; // the last thread takes the remaining rows
		ThreadList.emplace_back(func, i, GameMap, 0, GameWidth, yMin, yMax);
	}
	for (auto& th : ThreadList) { th.join(); }; // Wait for the Rectangles to be calculated
}


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
	GameWidth = ScreenWidth / GameScale;
	GameHeight = ScreenHeight / GameScale;
	ThreadCount = thread::hardware_concurrency();
	ThreadCountUsed; // Number of threads used
	ThreadCountUsed = ThreadCount * ThreadCountUsage;
	GameMap.resize(GameWidth, vector<int>(GameHeight));
	GameMapNext.resize(GameWidth, vector<int>(GameHeight));

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

