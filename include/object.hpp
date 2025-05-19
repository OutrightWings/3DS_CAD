#pragma once

typedef struct { float position[3]; float texcoord[2]; float normal[3]; } vertex;

extern vertex vertex_list[36];
#define vertex_list_count (sizeof(vertex_list)/sizeof(vertex_list[0]))
void generateTexturedCube(vertex* out, float u0, float u1, float v0, float v1);