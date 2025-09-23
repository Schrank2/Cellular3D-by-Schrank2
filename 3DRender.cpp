#include <iostream>
using namespace std;
#include <SDL3/SDL.h>
#include "defs.h"
#include "functions.h"
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <algorithm>

mutex renderLock;
vector<thread> RenderThreads;


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
struct Triangle {
	// Defining Components
	POS3D A, B, C;
	SDL_FColor color;
	// Defining Constructor
	Triangle(POS3D A, POS3D B, POS3D C, SDL_FColor color) : A(A), B(B), C(C), color(color) {}
};;
vector<Triangle> TriangleQueue; // Queue for Triangles

// Adding all Voxels to a list.
std::vector<Voxel> VoxelQueue;
static void readVoxels(const std::vector<std::vector<std::vector<int>>>& GameMap) {
	float ReadTime;
	if (Debug == true) { cout << "ReadVoxels started" << endl; ReadTime = SDL_GetTicks(); }
	// Clear the VoxelQueue
	VoxelQueue.clear();
	for (int i = 0; i < GameWidth; i++) {
		for (int j = 0; j < GameHeight; j++) {
			for (int k = 0; k < GameDepth; k++) {
				if (GameMap[i][j][k] == 1) {
					float d = 0.5f;
					if (k != 0) {
						d = 1.0f / k;
					}
					SDL_FColor color{ 0.0f , 0.0f , 0.0f , 1.0f }; // LATER UNUSED !!!
					POS3D pos(i, j, k);
					Voxel v = Voxel(pos, color);
					VoxelQueue.emplace_back(v);
				}
			}
		}
	}
	if (Debug == true) { cout << "ReadVoxels ended, took " << SDL_GetTicks() - ReadTime << "ms" << endl; }
}
static float ScreenCoordinateX(float x, float z) {
	float Depth = 1 + (0.03f * z); // Adjusting depth for perspective
	float scale = ScreenWidth / static_cast<float>(GameWidth);
	return (x / Depth) * scale;
}
static float ScreenCoordinateY(float y, float z) {
	float Depth = 1+ (0.03f * z); // Adjusting depth for perspective
	float scale = ScreenHeight / static_cast<float>(GameHeight);
	return (y / Depth) * scale;
}
void renderVoxel(Voxel V) {
	//cout << "Rendering Voxel at (" << V.position.x << ", " << V.position.y << ", " << V.position.z << ") with color (" << V.color.r << ", " << V.color.g << ", " << V.color.b << ", " << V.color.a << ")" << endl;
	vector<Triangle> Triangles;
	// Front Face
	Triangles.emplace_back(Triangle{{0,0,0},{0,1,0},{1,1,0},{1.0f,0.0f,0.0f,1.0f}});
	Triangles.emplace_back(Triangle{{0,0,0},{1,0,0},{1,1,0},{1.0f,0.0f,0.0f,1.0f}});
	// Back Face
	Triangles.emplace_back(Triangle{{0,0,1},{0,1,1},{1,1,1},{0.0f,1.0f,0.0f,1.0f}});
	Triangles.emplace_back(Triangle{{0,0,1},{1,0,1},{1,1,1},{0.0f,1.0f,0.0f,1.0f}});
	// Bottom Face
	Triangles.emplace_back(Triangle{{0,0,0},{1,0,0},{1,0,1},{0.0f,0.0f,1.0f,1.0f}});
	Triangles.emplace_back(Triangle{{0,0,0},{0,0,1},{1,0,1},{0.0f,0.0f,1.0f,1.0f}});
	// Top Face
	Triangles.emplace_back(Triangle{{0,1,0},{1,1,0},{1,1,1},{1.0f,1.0f,0.0f,1.0f}});
	Triangles.emplace_back(Triangle{{0,1,0},{0,1,1},{1,1,1},{1.0f,1.0f,0.0f,1.0f}});
	// Left Face
	Triangles.emplace_back(Triangle{{0,0,0},{0,1,0},{0,1,1},{0.0f,1.0f,1.0f,1.0f}});
	Triangles.emplace_back(Triangle{{0,0,0},{0,1,1},{0,0,1},{0.0f,1.0f,1.0f,1.0f}});
	// Right Face
	Triangles.emplace_back(Triangle{{1,0,0},{1,1,0},{1,1,1},{1.0f,0.0f,1.0f,1.0f}});
	Triangles.emplace_back(Triangle{{1,0,0},{1,1,1},{1,0,1},{1.0f,0.0f,1.0f,1.0f}});

	
	// The Loop for Drawing Triangles
	for (int i = 0; i < Triangles.size(); i++) {
		// Adjusting the position of the triangle based on the voxel position
		Triangles[i].A.x += V.position.x;
		Triangles[i].A.y += V.position.y;
		Triangles[i].A.z += V.position.z;
		Triangles[i].B.x += V.position.x;
		Triangles[i].B.y += V.position.y;
		Triangles[i].B.z += V.position.z;
		Triangles[i].C.x += V.position.x;
		Triangles[i].C.y += V.position.y;
		Triangles[i].C.z += V.position.z;
		TriangleQueue.emplace_back(Triangles[i]);
	}
};
static void DrawTriangle(Triangle T) {
	vector<SDL_Vertex> vertices(3);
	SDL_FPoint A = { ScreenCoordinateX(T.A.x,T.A.z),ScreenCoordinateY(T.A.y,T.A.z)};
	SDL_FPoint B = { ScreenCoordinateX(T.B.x,T.B.z),ScreenCoordinateY(T.B.y,T.B.z)};
	SDL_FPoint C = { ScreenCoordinateX(T.C.x,T.C.z),ScreenCoordinateY(T.C.y,T.C.z)};
	vertices[0].position = A;
	vertices[1].position = B;
	vertices[2].position = C;
	// Set the color of the vertices
	float c = min(T.A.z, min(T.B.z, T.C.z));
	if (c != 0.0f) { // Avoid division by zero
		c = 1 / c;
	} else {
		c = 1.0f;
	}
	SDL_FColor Color = { T.color.r * c,T.color.g * c,T.color.b * c,T.color.a };
	vertices[0].color = Color;
	vertices[1].color = Color;
	vertices[2].color = Color;
	vertices[0].tex_coord = { 0.0f, 0.0f };
	vertices[1].tex_coord = { 0.0f, 0.0f };
	vertices[2].tex_coord = { 0.0f, 0.0f };
	SDL_RenderGeometry(renderer, nullptr, vertices.data(), vertices.size(), nullptr, 0);
}

void renderThread(int Thread, int yMin, int yMax) {
	for (int i = yMin; i < yMax; i++) {
		renderLock.lock(); // Used to avoid Deadlock Issue
		renderVoxel(VoxelQueue[i]);
		renderLock.unlock();
	}
}
void render3D() {
	float render3DTime;
	readVoxels(GameMap);

	if (Debug == true) { cout << "Rendervoxel started" << endl; render3DTime = SDL_GetTicks(); }
	for (int i = 0; i < VoxelQueue.size(); i++) {
		renderVoxel(VoxelQueue[i]);
	}
	if (Debug == true) { cout << "ReadVoxels ended, took " << SDL_GetTicks() - render3DTime << "ms" << endl; }

	// Rendering Multithreaded
	if (Debug == true) { cout << "Rectangles started" << endl; render3DTime = SDL_GetTicks(); }
	RenderThreads.clear();
	int rowLength = GameHeight / ThreadCountUsed;
	for (int i = 0; i < ThreadCountUsed; i++) {
		int yMin = i * rowLength;
		int yMax = (i == ThreadCountUsed - 1) ? GameHeight : (i + 1) * rowLength; // the last thread takes the remaining rows
		// Start the thread to render the voxels
		RenderThreads.emplace_back(renderThread, i, yMin, yMax);

	}
	for (auto& th : RenderThreads) { th.join(); }; // Wait for the Rectangles to be calculated
	if (Debug == true) { cout << "Rectangles ended, took " << SDL_GetTicks() - render3DTime << "ms" << endl; }
	// Sort the Triangles by Depth
	if (Debug == true) { cout << "Depth sorting started" << endl; render3DTime = SDL_GetTicks(); }
	std::sort(TriangleQueue.begin(), TriangleQueue.end(), [](const Triangle& a, const Triangle& b) {
		float zA = (a.A.z + a.B.z + a.C.z) / 3.0f; // Average Z value of triangle A
		float zB = (b.A.z + b.B.z + b.C.z) / 3.0f; // Average Z value of triangle B
		return zA > zB; // Sort in descending order (farthest first)
		});
	if (Debug == true) { cout << "Depth Sorting ended, took " << SDL_GetTicks() - render3DTime << "ms" << endl; }
	// Render all Triangles
	for (int i = 0; i < TriangleQueue.size(); i++) {
		DrawTriangle(TriangleQueue[i]);
	}
	TriangleQueue.clear(); // Clear the Triangle Queue after rendering
}