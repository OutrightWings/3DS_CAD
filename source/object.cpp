#include "object.hpp"

vertex vertex_list[36];

void generateTexturedCube(vertex* out, float u0, float u1, float v0, float v1) {
    const float p = 0.5f;

    const struct {
        float normal[3];
        float corners[6][3]; // 6 vertices (2 triangles)
        float uvs[6][2];
    } faces[6] = {
        // Front (+Z)
        { {0, 0, +1}, {
            {-p, -p, +p}, {+p, -p, +p}, {+p, +p, +p},
            {+p, +p, +p}, {-p, +p, +p}, {-p, -p, +p}
        }, {
            {u0, v1}, {u1, v1}, {u1, v0},
            {u1, v0}, {u0, v0}, {u0, v1}
        }},
        // Back (-Z)
        { {0, 0, -1}, {
            {-p, -p, -p}, {-p, +p, -p}, {+p, +p, -p},
            {+p, +p, -p}, {+p, -p, -p}, {-p, -p, -p}
        }, {
            {u0, v1}, {u0, v0}, {u1, v0},
            {u1, v0}, {u1, v1}, {u0, v1}
        }},
        // Right (+X)
        { {+1, 0, 0}, {
            {+p, -p, -p}, {+p, +p, -p}, {+p, +p, +p},
            {+p, +p, +p}, {+p, -p, +p}, {+p, -p, -p}
        }, {
            {u0, v1}, {u0, v0}, {u1, v0},
            {u1, v0}, {u1, v1}, {u0, v1}
        }},
        // Left (-X)
        { {-1, 0, 0}, {
            {-p, -p, -p}, {-p, -p, +p}, {-p, +p, +p},
            {-p, +p, +p}, {-p, +p, -p}, {-p, -p, -p}
        }, {
            {u0, v1}, {u1, v1}, {u1, v0},
            {u1, v0}, {u0, v0}, {u0, v1}
        }},
        // Top (+Y)
        { {0, +1, 0}, {
            {-p, +p, -p}, {-p, +p, +p}, {+p, +p, +p},
            {+p, +p, +p}, {+p, +p, -p}, {-p, +p, -p}
        }, {
            {u0, v1}, {u1, v1}, {u1, v0},
            {u1, v0}, {u0, v0}, {u0, v1}
        }},
        // Bottom (-Y)
        { {0, -1, 0}, {
            {-p, -p, -p}, {+p, -p, -p}, {+p, -p, +p},
            {+p, -p, +p}, {-p, -p, +p}, {-p, -p, -p}
        }, {
            {u0, v1}, {u1, v1}, {u1, v0},
            {u1, v0}, {u0, v0}, {u0, v1}
        }},
    };

    for (int f = 0; f < 6; ++f) {
        for (int i = 0; i < 6; ++i) {
            int idx = f * 6 + i;
            for (int j = 0; j < 3; ++j)
                out[idx].position[j] = faces[f].corners[i][j];
            for (int j = 0; j < 2; ++j)
                out[idx].texcoord[j] = faces[f].uvs[i][j];
            for (int j = 0; j < 3; ++j)
                out[idx].normal[j] = faces[f].normal[j];
        }
    }
}