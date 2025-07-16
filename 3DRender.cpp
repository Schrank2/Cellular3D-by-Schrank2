#include <iostream>
using namespace std;
#include <SDL3/SDL.h>
#include "defs.h"
#include "functions.h"
#include <vector>
#include <string>

// Declaring the "Data Type" Voxel
struct RGBA {
	// Defining Components
	int r, g, b, a;
	// Defining Constructor
	RGBA(int r, int g, int b, int a) : r(r), g(g), b(b), a(a) {}
};;
struct POS {
	// Defining Components
	float x, y;
	// Defining Constructor
	POS(float x, float y) : x(x), y(y) {}
};
struct POS3D {
	// Defining Components
	float x, y, z;
	// Defining Constructor
	POS3D(float x, float y, float z) : x(x), y(y), z(z) {}
};
struct Triangle {
	// Defining Components
	POS p1, p2, p3;
	RGBA Color;
	// Defining Constructor
	Triangle(POS p1, POS p2, POS p3, RGBA Color) : p1(p1), p2(p2), p3(p3), Color(Color) {}
};
struct Voxel {
	// Defining Components
	POS3D Position;
	RGBA Color;
	// Defining Constructor
	Voxel(POS3D Position, RGBA Color) : Position(Position), Color(Color) {}
};

// Adding all Voxels to a list.
std::vector<Voxel> VoxelQueue;
void readVoxels(const std::vector<std::vector<int>>& GameMap) {
	// Clear the VoxelQueue
	VoxelQueue.clear();
	for (int i = 0; i < GameWidth; i++) {
		for (int j = 0; j < GameHeight; j++) {
			if (GameMap[i][j] == 1) {
				RGBA color(0, 255, 0, 255);
				POS3D pos(i, j, 3);
				Voxel v=Voxel(pos, color);
				VoxelQueue.emplace_back(v);
			}
		}
	}
}
int ScreenCoordinateX(int x, int z) {
	return ScreenWidth * x / z + ScreenWidth / 2;
}
int ScreenCoordinateY(int y, int z) {
	return -ScreenHeight * y / z + ScreenHeight / 2;
}
void renderVoxel(Voxel V) {
	cout << "Rendering Voxel at (" << V.Position.x << ", " << V.Position.y << ", " << V.Position.z << ") with color ("
		<< V.Color.r << ", " << V.Color.g << ", " << V.Color.b << ", " << V.Color.a << ")" << endl;
};
void render3D() {
	readVoxels(GameMap);
	for (int i = 0; i < VoxelQueue.size(); i++) {
		//cout << VoxelQueue[i].x << " " << VoxelQueue[i].y << " " << VoxelQueue[i].z << " " << VoxelQueue[i].Color[0] << " " << VoxelQueue[i].Color[1] << " " << VoxelQueue[i].Color[2] << " " << VoxelQueue[i].Color[3] << endl;
		renderVoxel(VoxelQueue[i]);
	}
}