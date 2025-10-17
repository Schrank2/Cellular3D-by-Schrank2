#include <SDL3/SDL.h>
#include <vector>
#include <thread>
#ifndef COMMON_H
#define COMMON_H
// Structs
struct POS3D {
	float x, y, z;
	POS3D(float x, float y, float z) : x(x), y(y), z(z) {}
};
struct Voxel {
	POS3D position;
	SDL_FColor color;
	Voxel(POS3D position, SDL_FColor color) : position(position), color(color) {}
};
struct Triangle {
	POS3D A, B, C;
	SDL_FColor color;
	Triangle(POS3D A, POS3D B, POS3D C, SDL_FColor color) : A(A), B(B), C(C), color(color) {}
};
struct TEXTUREMETA {
	SDL_Texture* texture;
	SDL_FRect rect;
	TEXTUREMETA(SDL_Texture* texture, SDL_FRect rect) : texture(texture), rect(rect) {}
};
struct KEYBOARD {
	bool W, A, S, D, LSHIFT, SPACE, C, V;
	KEYBOARD(bool W, bool A, bool S, bool D, bool LSHIFT, bool SPACE, bool C, bool V) : W(W),A(A),S(S),D(D),LSHIFT(LSHIFT),SPACE(SPACE), C(C), V(V) {}
};
// Misc
extern int CurrentTime;
extern int StartTime;
extern int LastTime;
extern int Toggle;
extern int GameWidth;
extern int GameHeight;
extern int GameDepth;
extern int ThreadCount;
extern int ThreadCountUsed;
extern bool Pause;
// Camera
struct Camera {
	float x;
	float y;
	float z;
	float vx;
	float vy;
	float vz;
	float pitch;
	float yaw;
	Camera(float x,float y, float z,float vx,float vy, float vz, float pitch, float yaw) : x(x),y(y),z(z),vx(vx),vy(vy),vz(vz),pitch(pitch),yaw(yaw) {}
};
extern Camera C1;

extern SDL_Window* window;
extern SDL_Texture* supersampleTex;
extern SDL_Renderer* renderer;
extern SDL_Event event;
extern std::vector<std::vector<std::vector<int>>> GameMap;
extern std::vector<std::vector<std::vector<int>>> GameMapNext;
extern std::vector<std::thread> ThreadPool;
// Debug Logs
extern float TaskTime;
extern float FrameTime;
extern float RenderTime;
extern float RenderInitTime;
extern float RenderVoxelTime;
extern float RenderPresentTime;
extern float RenderRectangleTime;
extern float RenderGeometryTime;
extern float TickTime;
extern float ReadVoxelTime;
extern float DepthSortTime;
extern float ProjectionTime;
extern float InputTime;
// Models
extern std::vector<Triangle> VoxelModel;
// Settings
extern int GameScale; // The Dimensions of each tile in pixels
extern float mapDensity; // The density of Cells in the beginning in 0.0 -> 1.0
extern int ScreenWidth; // The width of the game window in pixels
extern int ScreenHeight; // The height of the game window in pixels
extern float ThreadCountUsage; // The percentage of CPU threads to use for the game
extern int TickInterval; // Time between game ticks in milliseconds
extern bool Debug; // Should debug messages be printed to the console
extern float shadingStrength; // Strength of the shading based on depth
extern int AAScale; // Antialiasing strength, 1 = none, 2 = 2x, 3 = 3x, 4 = 4x
extern bool Multithreading;
#endif
