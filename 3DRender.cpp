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
	float Depth = 1 + (0.03f * z); // Adjusting depth for perspective
	float scale = ScreenWidth / static_cast<float>(GameWidth);
	return (x / Depth) * scale;
}
float ScreenCoordinateY(float y, float z) {
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
	vertices[0].color = T.color;
	vertices[1].color = T.color;
	vertices[2].color = T.color;
	vertices[0].tex_coord = { 0.0f, 0.0f };
	vertices[1].tex_coord = { 0.0f, 0.0f };
	vertices[2].tex_coord = { 0.0f, 0.0f };
	SDL_RenderGeometry(renderer, nullptr, vertices.data(), vertices.size(), nullptr, 0);
}

void render3D() {
	readVoxels(GameMap);
	for (int i = 0; i < VoxelQueue.size(); i++) {
		renderVoxel(VoxelQueue[i]);
	}
	// Sort the Triangles by Depth
	int comparison = 0;
	int i, j, s;
	Triangle tempTriangle = TriangleQueue[0]; // Temporary Variable for Swapping
	for (i = 0; i < TriangleQueue.size() - 1; i++) { // Durch die Liste Iterieren
		s = i; // Den Speicher auf die Ersten Position der Liste setzen.
		for (j = i; j < TriangleQueue.size(); j++) { // Ab dem Punkt in den der Erste Loop iteriert, alle restlichen Durchiterieren
			comparison = comparison + 1; // Counter um die Menge an Vergleichen zu zählen.
			if ((TriangleQueue[s].A.z + TriangleQueue[s].B.z + TriangleQueue[s].C.z) < (TriangleQueue[j].A.z + TriangleQueue[j].B.z + TriangleQueue[j].C.z)) { // Vergleichen zwischen dem Wert der Gespeicherten Position und dem vom 2. Loop iterierten Wert.
				s = j; // Speichern des 2.Loop-Werts, falls dieser größer als der bisherige Speicher ist.
			}
		}
		tempTriangle = TriangleQueue[s]; // Finales Tauschen des Zwischenspeichers mit dem ersten des zu bearbeitenden Bereich
		TriangleQueue[s] = TriangleQueue[i];
		TriangleQueue[i] = tempTriangle;
	};
	// Render all Triangles
	for (int i = 0; i < TriangleQueue.size(); i++) {
		DrawTriangle(TriangleQueue[i]);
	}
	TriangleQueue.clear(); // Clear the Triangle Queue after rendering
}