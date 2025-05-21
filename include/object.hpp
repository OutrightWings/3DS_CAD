#pragma once
#include "modelObject.hpp"

extern Vertex vertex_list[36];
#define vertex_list_count (sizeof(vertex_list)/sizeof(vertex_list[0]))
void generateTexturedCube(Vertex* out, float u0, float u1, float v0, float v1);