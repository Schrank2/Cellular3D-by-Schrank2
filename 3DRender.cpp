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
#include <tuple>
mutex renderLock;
vector<thread> RenderThreads;
vector<TEXTUREMETA> TriangleTextures;
vector<Triangle> TriangleQueue; // Queue for Triangles
// Adding all Voxels to a list.
std::vector<Voxel> VoxelQueue;
inline static void readVoxels(const std::vector<std::vector<std::vector<int>>>& GameMap) {
	if (Debug == true) {ReadVoxelTime = SDL_GetTicks();}
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
					POS3D pos(i-(GameWidth*0.5), j-(GameHeight*0.5), k-(GameDepth*0.5));
					Voxel v = Voxel(pos, color);
					VoxelQueue.emplace_back(v);
				}
			}
		}
	}
	if (Debug == true) {ReadVoxelTime = SDL_GetTicks() - ReadVoxelTime; }
}
inline static float ScreenCoordinateX(float x, float z) {
	float Depth = 1+z-CameraZ; // Adjusting depth for perspective
	int a = ScreenWidth * (x + CameraX) / Depth;
	int offset = ScreenWidth * 0.5;
	return  a + offset;
}
inline static float ScreenCoordinateY(float y, float z) {
	float Depth = 1+z-CameraZ; // Adjusting depth for perspective
	int a = ScreenWidth * (y + CameraY) / Depth;
	int offset = ScreenHeight * 0.5;
	return  a + offset;
}
inline static void renderModel(Voxel V) {
	// Loading the Voxel Model
	vector<Triangle> Triangles = VoxelModel;
	// The Loop for Offsetting Triangles
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
inline static float GetDepthDark(float A) {
	int CameraDistance = A - CameraZ;
	A = shadingStrength * (CameraDistance + 1.0f);
	if (A == 0) { A = 1.0f; }
	A = 1 / A;
	return A;
}
inline static bool DrawTriangle(Triangle T) {
	vector<SDL_Vertex> vertices(3);
	SDL_FPoint A = {AAScale*ScreenCoordinateX(T.A.x,T.A.z), AAScale*ScreenCoordinateY(T.A.y,T.A.z)};
	SDL_FPoint B = {AAScale*ScreenCoordinateX(T.B.x,T.B.z), AAScale*ScreenCoordinateY(T.B.y,T.B.z)};
	SDL_FPoint C = {AAScale*ScreenCoordinateX(T.C.x,T.C.z), AAScale*ScreenCoordinateY(T.C.y,T.C.z)};
	float c = GetDepthDark(T.A.z);
	vertices[0].color = { T.color.r * c,T.color.g * c,T.color.b * c,T.color.a };
	c = GetDepthDark(T.B.z);
	vertices[1].color = { T.color.r * c,T.color.g * c,T.color.b * c,T.color.a };
	c = GetDepthDark(T.C.z);
	vertices[2].color = { T.color.r * c,T.color.g * c,T.color.b * c,T.color.a };
	vertices[0].position = A;
	vertices[1].position = B;
	vertices[2].position = C;
	// Render to the Supersample Texture
	SDL_SetRenderTarget(renderer, supersampleTex);
	SDL_RenderGeometry(renderer, nullptr, vertices.data(), 3, nullptr, 0);
	return true;
}

static void renderThread(int Thread, int yMin, int yMax) {
	for (int i = yMin; i < yMax; i++) {
		renderLock.lock(); // Used to avoid Deadlock Issue
		renderModel(VoxelQueue[i]);
		renderLock.unlock();
	}
}
void render3D() {
	// Zwischentextur für Antialiasing (Supersampling) resetten
	SDL_SetRenderTarget(renderer, supersampleTex);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer); // Clear the Texture with white color

	readVoxels(GameMap);
	if (Debug == true) {RenderVoxelTime = SDL_GetTicks();}
	for (int i = 0; i < VoxelQueue.size(); i++) {
		renderModel(VoxelQueue[i]);
	}
	if (Debug == true) {RenderVoxelTime = SDL_GetTicks() - RenderVoxelTime;}

	// Rendering Multithreaded
	if (Debug == true) {RenderRectangleTime = SDL_GetTicks();}
	RenderThreads.clear();
	int rowLength = GameHeight / ThreadCountUsed;
	for (int i = 0; i < ThreadCountUsed; i++) {
		int yMin = i * rowLength;
		int yMax = (i == ThreadCountUsed - 1) ? GameHeight : (i + 1) * rowLength; // the last thread takes the remaining rows
		// Start the thread to render the voxels
		RenderThreads.emplace_back(renderThread, i, yMin, yMax);

	}
	for (auto& th : RenderThreads) { th.join(); }; // Wait for the Rectangles to be calculated
	if (Debug == true) {RenderRectangleTime = SDL_GetTicks() - RenderRectangleTime;}
	// Sort the Triangles by Depth
	if (Debug == true) {DepthSortTime = SDL_GetTicks(); }
	std::sort(TriangleQueue.begin(), TriangleQueue.end(), [](const Triangle& a, const Triangle& b) {
		float zA = (a.A.z + a.B.z + a.C.z) / 3.0f; // Average Z value of triangle A
		float zB = (b.A.z + b.B.z + b.C.z) / 3.0f; // Average Z value of triangle B
		return zA > zB; // Sort in descending order (farthest first)
		});
	if (Debug == true) {DepthSortTime = SDL_GetTicks() - DepthSortTime; }
	// Render all Triangles
	if (Debug == true) { DrawTime = SDL_GetTicks();}
	for (int i = 0; i < TriangleQueue.size(); i++) {
		DrawTriangle(TriangleQueue[i]);
	}
	TriangleQueue.clear(); // Clear the Triangle Queue after rendering
	if (Debug == true) { DrawTime = SDL_GetTicks() - DrawTime; }
	TriangleTextures.clear();
	// Draw the Supersampled Texture to the screen
	SDL_SetRenderTarget(renderer, nullptr);
	SDL_FRect rect = {0,0,ScreenWidth,ScreenHeight};
	SDL_RenderTexture(renderer, supersampleTex, nullptr, &rect);
}