#include <iostream>
using namespace std;
#include <SDL3/SDL.h>
#include "defs.h"
#include "functions.h"
#include <vector>
#include <string>
// Declaring the "Data Type" Voxel
struct Voxel {
	// Defining Components
	float x, y, z;
	std::vector<int> Color;
	// Defining Constructor
	Voxel(float x, float y, float z, std::vector<int> Color) : x(x), y(y), z(z), Color(Color) {}
};
// Adding all Voxels to a list.
std::vector<Voxel> VoxelQueue;
void readVoxels(const std::vector<std::vector<int>>& GameMap) {
	// Clear the VoxelQueue
	VoxelQueue.clear();
	for (int i = 0; i < GameWidth; i++) {
		for (int j = 0; j < GameHeight; j++) {
			if (GameMap[i][j] == 1) {
				Voxel v=Voxel(i, j, 0, { 0, 255, 0, 0 });
				VoxelQueue.emplace_back(v);
			}
		}
	}
}
void renderVoxel() {};
void render3D() {
	readVoxels(GameMap);
	for (int i = 0; i < VoxelQueue.size(); i++) {
		//cout << VoxelQueue[i].x << " " << VoxelQueue[i].y << " " << VoxelQueue[i].z << " " << VoxelQueue[i].Color[0] << " " << VoxelQueue[i].Color[1] << " " << VoxelQueue[i].Color[2] << " " << VoxelQueue[i].Color[3] << endl;
		renderVoxel(VoxelQueue[i]);
	}
}