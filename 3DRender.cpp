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
#include <array>
mutex renderLock;
vector<thread> RenderThreads;
// Adding all Voxels to a list.
vector<Voxel> VoxelQueue;
vector<Triangle> TriangleQueue; // Queue for Triangles
// Queue for the final triangles (first vector is for the Thread, second for the Triangle and third for the Vertex)
vector<vector<SDL_Vertex>> VerticieQueue; // VerticieQueue for Multithreading
vector<SDL_Vertex> VerticieQueueS; // VerticieQueue for Singlethreading

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
	float Depth = 1+z-C1.z; // Adjusting depth for perspective
	int a = ScreenWidth * (x + C1.x) / Depth;
	int offset = ScreenWidth * 0.5;
	return  a + offset;
}
inline static void rotatePoint(POS3D P, Camera C1) {

}
inline static void rotateThread(int Min, int Max) {
	for (int i = Min; i < Max; i++) {
		// Rotate each vertex of the triangle
		TriangleQueue[i].A = RotatePoint(TriangleQueue[i].A, C1);
		TriangleQueue[i].B = RotatePoint(TriangleQueue[i].B, C1);
		TriangleQueue[i].C = RotatePoint(TriangleQueue[i].C, C1);
	}
}
inline static vector<Triangle> RotateScene(vector<Triangle> TriangleQueue) {
	int rowLengthTriangle = TriangleQueue.size() / ThreadCountUsed;
	for (int i = 0; i < ThreadCountUsed; i++) {
		int Min = i * rowLengthTriangle;
		int Max = (i == ThreadCountUsed - 1) ? GameHeight : (i + 1) * rowLengthTriangle; // the last thread takes the remaining rows
		// Start the thread to render the voxels
		ThreadPool[i] = thread(rotateThread, Min, Max);
	}
	for (auto& th : ThreadPool) { th.join(); }; // Wait for the Rectangles to be calculated
	return TriangleQueue;
}
inline static float ScreenCoordinateY(float y, float z) {
	float Depth = 1+z-C1.z; // Adjusting depth for perspective
	int a = ScreenWidth * (y + C1.y) / Depth;
	int offset = ScreenHeight * 0.5;
	return  -a + offset;
}
// Condition for Triangle Culling
inline static bool culled(Triangle T) { // TBH heavy Autopilot usage here
	// Before Camera Culling
	if (T.A.z < C1.z && T.B.z < C1.z && T.C.z < C1.z) { return true; }
	// Backface culling
	// Compute two edges
	float ux = T.B.x - T.A.x; float uy = T.B.y - T.A.y; float uz = T.B.z - T.A.z;
	float vx = T.C.x - T.A.x; float vy = T.C.y - T.A.y; float vz = T.C.z - T.A.z;
	// Compute normal (cross product)
	float nx = uy * vz - uz * vy; float ny = uz * vx - ux * vz; float nz = ux * vy - uy * vx;
	// View direction (from camera to vertex A)
	float viewx = T.A.x - C1.x; float viewy = T.A.y - C1.y; float viewz = T.A.z - C1.z;
	// Dot product
	float dot = nx * viewx + ny * viewy + nz * viewz;
	// Cull if the triangle is facing away from the camera
	return false;//(dot >= 0.0f);
}
inline static void renderModel(Voxel V) {
	// Loading the Voxel Model
	vector<Triangle> Triangles = VoxelModel;
	// The Loop for Offsetting Triangles
	for (int i = 0; i < Triangles.size(); i++) {
		// Adjusting the position of the triangle based on the Models position
		Triangles[i].A.x += V.position.x;
		Triangles[i].A.y += V.position.y;
		Triangles[i].A.z += V.position.z;
		Triangles[i].B.x += V.position.x;
		Triangles[i].B.y += V.position.y;
		Triangles[i].B.z += V.position.z;
		Triangles[i].C.x += V.position.x;
		Triangles[i].C.y += V.position.y;
		Triangles[i].C.z += V.position.z;
		if (!culled(Triangles[i])) {
			TriangleQueue.emplace_back(Triangles[i]);
		}
	}
};
inline static float GetDepthDark(float A) {
	int CameraDistance = A - C1.z;
	A = shadingStrength * (CameraDistance + 1.0f);
	if (A == 0) { A = 1.0f; }
	A = 1 / A;
	return A;
}
inline static array<SDL_Vertex, 3> DrawTriangle(Triangle T) {
	array<SDL_Vertex, 3> Vert;
	float c;
	Vert[0].position = {AAScale * ScreenCoordinateX(T.A.x,T.A.z), AAScale * ScreenCoordinateY(T.A.y,T.A.z)};
	Vert[1].position = {AAScale * ScreenCoordinateX(T.B.x,T.B.z), AAScale * ScreenCoordinateY(T.B.y,T.B.z)};
	Vert[2].position = {AAScale * ScreenCoordinateX(T.C.x,T.C.z), AAScale * ScreenCoordinateY(T.C.y,T.C.z)};
	c = GetDepthDark(T.A.z);
	Vert[0].color = {T.color.r * c,T.color.g * c,T.color.b * c,T.color.a};
	c = GetDepthDark(T.B.z);
	Vert[1].color = {T.color.r * c,T.color.g * c,T.color.b * c,T.color.a};
	c = GetDepthDark(T.C.z);
	Vert[2].color = {T.color.r * c,T.color.g * c,T.color.b * c,T.color.a};
	return Vert;
}
inline static void ProjectionThread(int Min, int Max, int Thread) { // some Autopilot but I tried to understand it
	vector<SDL_Vertex> temp;
	temp.reserve((Max - Min) * 3); // avoid misallocations
	for (int i = Min; i < Max; i++) {
		array<SDL_Vertex, 3> verts = DrawTriangle(TriangleQueue[i]); // returns 3 vertices
		temp.push_back(verts[0]);
		temp.push_back(verts[1]);
		temp.push_back(verts[2]);
	}
	// move is faster than a copy, therefore:
	VerticieQueue[Thread] = move(temp); // move temp to the correct position in VerticieQueue
	// so I just dont put each triangle into its own vector, but I just reserve memory for the three verticies
}
inline static void ProjectionMultithreaded() {
	// Clear the Verticie Queue
	VerticieQueue.clear();
	VerticieQueue.resize(ThreadCountUsed);
	int rowLength = TriangleQueue.size() / ThreadCountUsed;
	for (int i = 0; i < ThreadCountUsed; i++) {
		ThreadPool[i] = thread(ProjectionThread, i*rowLength, (i+1) * rowLength, i);
	}
	for (auto& th : ThreadPool) { th.join(); };
}
inline static void ProjectionSinglethreaded() { // inspired by Autopilot in Projectionmultithreaded, but no direct Autoplilot here
	// Clear the Verticie Queue
	VerticieQueueS.clear();
	vector<SDL_Vertex> temp;
	VerticieQueueS.reserve(TriangleQueue.size() * 3); // resize memory for all verticies
	for (int i = 0; i < TriangleQueue.size(); i++) {
		array<SDL_Vertex, 3> verts = DrawTriangle(TriangleQueue[i]);
		temp.push_back(verts[0]);
		temp.push_back(verts[1]);
		temp.push_back(verts[2]);
		VerticieQueueS.emplace_back(verts[0]);
		VerticieQueueS.emplace_back(verts[1]);
		VerticieQueueS.emplace_back(verts[2]);
	}
}
static void renderThread(int yMin, int yMax) {
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
	// reading the Voxels
	readVoxels(GameMap);
	// Rendering Multithreaded
	if (Debug == true) { RenderRectangleTime = SDL_GetTicks(); }
	int rowLengthVoxel =  VoxelQueue.size() / ThreadCountUsed;
	for (int i = 0; i < ThreadCountUsed; i++) {
		int yMin = i * rowLengthVoxel;
		int yMax = (i == ThreadCountUsed - 1) ? GameHeight : (i + 1) * rowLengthVoxel; // the last thread takes the remaining rows
		// Start the thread to render the voxels
		ThreadPool[i] = thread(renderThread, yMin, yMax);
	}
	for (auto& th : ThreadPool) { th.join(); }; // Wait for the Rectangles to be calculated
	if (Debug == true) { RenderRectangleTime = SDL_GetTicks() - RenderRectangleTime; }
	// rotating all the Triangles, so that the Camera is now the "rotation center"
	RotateScene(TriangleQueue);
	// Sort the Triangles by Depth
	if (Debug == true) { DepthSortTime = SDL_GetTicks(); }
	std::sort(TriangleQueue.begin(), TriangleQueue.end(), [](const Triangle& a, const Triangle& b) {
		float AVGzA = (a.A.z + a.B.z + a.C.z) / 3.0f;
		float AVGzB = (b.A.z + b.B.z + b.C.z) / 3.0f;
		return AVGzA > AVGzB;
		});
	if (Debug == true) { DepthSortTime = SDL_GetTicks() - DepthSortTime; }

	// 2D-Project all Triangles
	if (Debug == true) { ProjectionTime = SDL_GetTicks(); } // Projection Time Start
	ProjectionMultithreaded();
	//ProjectionSinglethreaded();
	TriangleQueue.clear(); // Clear the Triangle Queue
	SDL_SetRenderTarget(renderer, supersampleTex);
	if (Debug == true) { ProjectionTime = SDL_GetTicks() - ProjectionTime; } // Projection Time End

	if (Debug == true) { RenderGeometryTime = SDL_GetTicks(); } // Final RenderGeometry Time Start
	if (Multithreading){
		for (int t = 0; t < VerticieQueue.size(); t++) {
			SDL_RenderGeometry(renderer, nullptr, VerticieQueue[t].data(), VerticieQueue[t].size(), nullptr, 0);
		}
	}
	if (!Multithreading){ 
		SDL_RenderGeometry(renderer, nullptr, VerticieQueueS.data(), VerticieQueueS.size(), nullptr, 0);
	}
	
	if (Debug == true) { RenderGeometryTime = SDL_GetTicks() - RenderGeometryTime; } // Final Rendergeometry Time End
	
	// Draw the Supersampled Texture to the screen
	SDL_SetRenderTarget(renderer, nullptr);
	SDL_FRect rect = { 0,0,ScreenWidth,ScreenHeight };
	SDL_RenderTexture(renderer, supersampleTex, nullptr, &rect);
}