#pragma once
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include <cstdlib>
typedef struct { float pos[3]; float uv[2]; float nm[3]; } Vertex;
struct FaceVertex {
    Vertex* v;
    float uv[2];
};
typedef std::vector<FaceVertex> Face;

class Model {
    public:
    std::vector<Vertex> vertices;                          // Master list of vertex data
    std::vector<Face> faces;              // Faces (polygonal), made of pointers to vertices

    Vertex* triArray = nullptr;                           // C-style array of triangle vertex data
    size_t triCount = 0;                                  // Number of vertices in triArray (always multiple of 3)
    std::vector<Vertex*> triSourcePointers;               // Tracks which original vertex each tri vertex came from

    // Constructor initializes the model with a cube
    Model() {
        initCube();
        generateTris();
    }

    // Destructor frees triangle array memory
    ~Model() {
        freeTriArray();
    }

    // Frees triangle array and associated tracking data
    void freeTriArray();

    // Initializes the model as a unit cube centered at origin
    void initCube();

    // Adds a vertex between two connected ones if they share an edge in a face
    Vertex* addMidpoint(Vertex* a, Vertex* b);

    // Computes and returns the unit normal of the triangle (v0, v1, v2)
    std::array<float, 3> computeNormal(Vertex* v0, Vertex* v1, Vertex* v2);

    // Triangulates all faces and generates triArray + triSourcePointers
    void generateTris();

    // Updates a vertex's position and recalculates normals for affected tris
    void updateVertex(Vertex* v, float x, float y, float z);
};