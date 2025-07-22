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
	POS3D position;
	SDL_FColor color;
	// Defining Constructor
	Voxel(POS3D position, SDL_FColor color) : position(position), color(color) {}
};

// Adding all Voxels to a list.
std::vector<Voxel> VoxelQueue;
void readVoxels(const std::vector<std::vector<int>>& GameMap) {
	// Clear the VoxelQueue
	VoxelQueue.clear();
	for (int i = 0; i < GameWidth; i++) {
		for (int j = 0; j < GameHeight; j++) {
			if (GameMap[i][j] == 1) {
				SDL_FColor color{0.0f, 0.5f, 0.0f, 1.0f};
				POS3D pos(i, j, 1);
				Voxel v=Voxel(pos, color);
				VoxelQueue.emplace_back(v);
			}
		}
	}
}
float ScreenCoordinateX(float x, float z) {
	float scale = ScreenWidth / static_cast<float>(GameWidth);
	return (x / z) * scale;
}
float ScreenCoordinateY(float y, float z) {
	float scale = ScreenHeight / static_cast<float>(GameHeight);
	return (y / z) * scale;
}
void renderVoxel(Voxel V) {
	//cout << "Rendering Voxel at (" << V.position.x << ", " << V.position.y << ", " << V.position.z << ") with color (" << V.color.r << ", " << V.color.g << ", " << V.color.b << ", " << V.color.a << ")" << endl;
	vector<SDL_Vertex> vertices(3);
	
	SDL_FPoint A = {ScreenCoordinateX(V.position.x, V.position.z), ScreenCoordinateY(V.position.y, V.position.z)};
	SDL_FPoint B = {ScreenCoordinateX((V.position.x+1), V.position.z), ScreenCoordinateY(V.position.y, V.position.z)};
	SDL_FPoint C = {ScreenCoordinateX((V.position.x+1), V.position.z), ScreenCoordinateY((V.position.y+1), V.position.z)};
	vertices[0].position = A;
	vertices[1].position = B;
	vertices[2].position = C;
	vertices[0].color = V.color;
	vertices[1].color = V.color;
	vertices[2].color = V.color;
	vertices[0].tex_coord = { 0.0f, 0.0f };
	vertices[1].tex_coord = { 0.0f, 0.0f };
	vertices[2].tex_coord = { 0.0f, 0.0f };
	SDL_RenderGeometry(renderer, nullptr, vertices.data(), vertices.size(), nullptr, 0); // Clear previous geometry
};
void render3D() {
	readVoxels(GameMap);
	for (int i = 0; i < VoxelQueue.size(); i++) {
		//cout << VoxelQueue[i].x << " " << VoxelQueue[i].y << " " << VoxelQueue[i].z << " " << VoxelQueue[i].Color[0] << " " << VoxelQueue[i].Color[1] << " " << VoxelQueue[i].Color[2] << " " << VoxelQueue[i].Color[3] << endl;
		renderVoxel(VoxelQueue[i]);
	}
}