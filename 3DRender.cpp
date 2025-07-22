#include <iostream>
using namespace std;
#include <SDL3/SDL.h>
#include "defs.h"
#include "functions.h"
#include <vector>
#include <string>

// Declaring the "Data Type" Voxel
struct POS3D {
	// Defining Components
	float x, y, z;
	// Defining Constructor
	POS3D(float x, float y, float z) : x(x), y(y), z(z) {}
};
struct Voxel {
	// Defining Components
	POS3D Position;
	SDL_Color Color;
	// Defining Constructor
	Voxel(POS3D Position, SDL_Color Color) : Position(Position), Color(Color) {}
};

// Adding all Voxels to a list.
std::vector<Voxel> VoxelQueue;
void readVoxels(const std::vector<std::vector<int>>& GameMap) {
	// Clear the VoxelQueue
	VoxelQueue.clear();
	for (int i = 0; i < GameWidth; i++) {
		for (int j = 0; j < GameHeight; j++) {
			if (GameMap[i][j] == 1) {
				SDL_Color color(0, 255, 0, 255);
				POS3D pos(i, j, 3);
				Voxel v=Voxel(pos, color);
				VoxelQueue.emplace_back(v);
			}
		}
	}
}
float ScreenCoordinateX(float x, float z) {
	return ScreenWidth * x / z + ScreenWidth / 2;
}
float ScreenCoordinateY(float y, float z) {
	return -ScreenHeight * y / z + ScreenHeight / 2;
}
void renderVoxel(Voxel V) {
	cout << "Rendering Voxel at (" << V.Position.x << ", " << V.Position.y << ", " << V.Position.z << ") with color ("
		<< V.Color.r << ", " << V.Color.g << ", " << V.Color.b << ", " << V.Color.a << ")" << endl;
	vector<SDL_Vertex> vertices(3);
	SDL_FPoint A = {ScreenCoordinateX(V.Position.x, V.Position.z), ScreenCoordinateY(V.Position.y, V.Position.z)};
	SDL_FPoint B = {ScreenCoordinateX(V.Position.x, V.Position.z), ScreenCoordinateY(V.Position.y, V.Position.z)};
	SDL_FPoint C = {ScreenCoordinateX(V.Position.x, V.Position.z), ScreenCoordinateY(V.Position.y, V.Position.z)};
	vertices[0].position = A;
	vertices[1].position = B;
	vertices[2].position = C;
	vertices[0].color = V.Color;
	vertices[1].color = V.Color;
	vertices[2].color = V.Color;
	SDL_RenderGeometry(renderer, nullptr, vertices.data(), vertices.size(), nullptr, 0); // Clear previous geometry
};
void render3D() {
	readVoxels(GameMap);
	for (int i = 0; i < VoxelQueue.size(); i++) {
		//cout << VoxelQueue[i].x << " " << VoxelQueue[i].y << " " << VoxelQueue[i].z << " " << VoxelQueue[i].Color[0] << " " << VoxelQueue[i].Color[1] << " " << VoxelQueue[i].Color[2] << " " << VoxelQueue[i].Color[3] << endl;
		renderVoxel(VoxelQueue[i]);
	}
}