#include "modelObject.hpp"

Model* model;
C2D_Sprite modelSprite;

void initModel(){
    model = new Model();
    C2D_SpriteFromSheet(&modelSprite, getModelSheet(), 0);
}

void Model::freeTriArray() {
    if (triArray) {
        delete[] triArray;
        triArray = nullptr;
        triCount = 0;
        triSourcePointers.clear();
    }
}
void Model::initCube() {
    vertices.clear();
    faces.clear();

    float p[8][3] = {
        {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f},
        {-0.5f, -0.5f,  0.5f}, {0.5f, -0.5f,  0.5f}, {0.5f, 0.5f,  0.5f}, {-0.5f, 0.5f,  0.5f}
    };

    for (int i = 0; i < 8; ++i) {
        Vertex v;
        std::copy(p[i], p[i] + 3, v.pos);
        v.uv[0] = v.uv[1] = 0; // unused now
        v.nm[0] = v.nm[1] = v.nm[2] = 0;
        vertices.push_back(v);
    }

    // UV layout per face (square)
    float uvs[4][2] = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
    };

    int quads[6][4] = {
        {0, 1, 2, 3}, {4, 5, 6, 7},
        {0, 1, 5, 4}, {2, 3, 7, 6},
        {0, 3, 7, 4}, {1, 2, 6, 5}
    };

    for (int f = 0; f < 6; ++f) {
        Face face;
        for (int i = 0; i < 4; ++i) {
            FaceVertex fv;
            fv.v = &vertices[quads[f][i]];
            fv.uv[0] = uvs[i][0];
            fv.uv[1] = uvs[i][1];
            face.push_back(fv);
        }
        faces.push_back(face);
    }
}
Vertex* Model::addMidpoint(Vertex* a, Vertex* b) {
    bool edgeUsed = false;
    float midUV[2] = {0.0f, 0.0f};
    int uvSamples = 0;

    for (auto& face : faces) {
        for (size_t i = 0; i < face.size(); ++i) {
            FaceVertex& fv1 = face[i];
            FaceVertex& fv2 = face[(i + 1) % face.size()];

            if ((fv1.v == a && fv2.v == b) || (fv1.v == b && fv2.v == a)) {
                edgeUsed = true;

                for (int j = 0; j < 2; ++j) {
                    midUV[j] += (fv1.uv[j] + fv2.uv[j]) * 0.5f;
                }
                uvSamples++;
            }
        }
    }

    if (!edgeUsed) return nullptr;

    for (int j = 0; j < 2; ++j)
        midUV[j] /= std::max(uvSamples, 1); // average if multiple faces

    // Create new midpoint vertex
    Vertex mid;
    for (int i = 0; i < 3; ++i)
        mid.pos[i] = (a->pos[i] + b->pos[i]) * 0.5f;
    mid.uv[0] = midUV[0];
    mid.uv[1] = midUV[1];

    vertices.push_back(mid);
    Vertex* midPtr = &vertices.back();

    // Insert into the faces that share the edge
    for (auto& face : faces) {
        for (size_t i = 0; i < face.size(); ++i) {
            FaceVertex& fv1 = face[i];
            FaceVertex& fv2 = face[(i + 1) % face.size()];

            if ((fv1.v == a && fv2.v == b) || (fv1.v == b && fv2.v == a)) {
                FaceVertex newFV;
                newFV.v = midPtr;
                newFV.uv[0] = (fv1.uv[0] + fv2.uv[0]) * 0.5f;
                newFV.uv[1] = (fv1.uv[1] + fv2.uv[1]) * 0.5f;

                face.insert(face.begin() + ((i + 1) % face.size()), newFV);
                break;
            }
        }
    }

    return midPtr;
}

std::array<float, 3> Model::computeNormal(Vertex* v0, Vertex* v1, Vertex* v2) {
    float u[3], v[3], n[3];
    for (int i = 0; i < 3; ++i) {
        u[i] = v1->pos[i] - v0->pos[i];
        v[i] = v2->pos[i] - v0->pos[i];
    }

    n[0] = u[1] * v[2] - u[2] * v[1];
    n[1] = u[2] * v[0] - u[0] * v[2];
    n[2] = u[0] * v[1] - u[1] * v[0];

    float len = std::sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
    if (len > 1e-6f) {
        for (int i = 0; i < 3; ++i) n[i] /= len;
    } else {
        n[0] = n[1] = n[2] = 0;
    }

    return {n[0], n[1], n[2]};
}
void Model::generateTris() {
    freeTriArray();
    std::vector<Vertex> tempTris;
    triSourcePointers.clear();

    for (const auto& face : faces) {
        if (face.size() < 3) continue;

        for (size_t i = 1; i + 1 < face.size(); ++i) {
            FaceVertex fv0 = face[0];
            FaceVertex fv1 = face[i];
            FaceVertex fv2 = face[i + 1];

            std::array<float, 3> normal = computeNormal(fv0.v, fv1.v, fv2.v);

            Vertex v0 = *fv0.v;
            Vertex v1 = *fv1.v;
            Vertex v2 = *fv2.v;

            std::copy(normal.begin(), normal.end(), v0.nm);
            std::copy(normal.begin(), normal.end(), v1.nm);
            std::copy(normal.begin(), normal.end(), v2.nm);

            std::copy(fv0.uv, fv0.uv + 2, v0.uv);
            std::copy(fv1.uv, fv1.uv + 2, v1.uv);
            std::copy(fv2.uv, fv2.uv + 2, v2.uv);

            tempTris.push_back(v0);
            tempTris.push_back(v1);
            tempTris.push_back(v2);

            triSourcePointers.push_back(fv0.v);
            triSourcePointers.push_back(fv1.v);
            triSourcePointers.push_back(fv2.v);
        }
    }

    triCount = tempTris.size();
    triArray = new Vertex[triCount];
    std::copy(tempTris.begin(), tempTris.end(), triArray);
}

void Model::updateVertex(Vertex* v) {
    for (size_t i = 0; i < triCount; i += 3) {
        Vertex* v0 = triSourcePointers[i];
        Vertex* v1 = triSourcePointers[i + 1];
        Vertex* v2 = triSourcePointers[i + 2];

        if (v0 == v || v1 == v || v2 == v) {
            std::array<float, 3> normal = computeNormal(v0, v1, v2);

            for (int j = 0; j < 3; ++j) {
                Vertex* src = triSourcePointers[i + j];
                for (int k = 0; k < 3; ++k) {
                    triArray[i + j].pos[k] = src->pos[k];
                    triArray[i + j].nm[k]  = normal[k];
                }
                for (int k = 0; k < 2; ++k) {
                    triArray[i + j].uv[k] = src->uv[k];
                }
            }
        }
    }
}