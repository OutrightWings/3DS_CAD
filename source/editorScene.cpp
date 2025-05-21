#include "scenes.hpp"
#include "spriteSheet.hpp"
#include "graphics.hpp"
#include "object.hpp"
#include <string>
#include <algorithm>

static C2D_TextBuf staticTextBuf;
static C2D_Text txt;
static int c1[2] = {0, 0};       // top-left corner of the projection area
static int c2[2] = {320, 240};   // bottom-right corner of the projection area
static bool dragged = false;
Vertex* selectedVertex = nullptr;
const float VERTEX_RADIUS = 6.0f;

void SceneEditor::init(){
    C2D_SpriteFromSheet(&viewSprite, getSpriteSheet(), 0);
    C2D_SpriteSetCenter(&viewSprite, 0, 0);
    C2D_SpriteSetPos(&viewSprite, 0, 0);

    C2D_SpriteFromSheet(&modelSprite, getModelSheet(), 0);
    
    model = new Model();
    updateVBO(model->triArray,model->triCount);
    state = VIEW_TOP;

    staticTextBuf = C2D_TextBufNew(128);
	C2D_TextParse(&txt, staticTextBuf, std::to_string(model->triCount).c_str());
	C2D_TextOptimize(&txt);
}
bool SceneEditor::userInput(){
    bool exit;
    exit = handleKeys();
    handleTouch();

    return exit;
}
void SceneEditor::handleTouch(){
    touchPosition touch;
    hidTouchRead(&touch);

    if (!(touch.px == 0 && touch.py == 0)) { // screen is touched
        if (!dragged) {
            // We are just starting to drag: find the vertex under the touch point
            selectedVertex = nullptr;
            float bestDistSq = VERTEX_RADIUS * VERTEX_RADIUS;

            for (Vertex& v : model->vertices) {
                auto [vx, vy] = projectTo2D(&v);
                float dx = vx - touch.px;
                float dy = vy - touch.py;
                float distSq = dx * dx + dy * dy;

                if (distSq <= bestDistSq) {
                    bestDistSq = distSq;
                    selectedVertex = &v;
                }
            }
        }

        dragged = true;

        if (selectedVertex) {
            std::array<float, 3> newPos = screenToModel(touch.px, touch.py, selectedVertex);

            // Clamp to model space bounds (e.g. [-1, 1])
            switch (state) {
                case VIEW_TOP:
                    newPos[0] = clamp(newPos[0], -1.0f, 1.0f);
                    newPos[1] = clamp(newPos[1], -1.0f, 1.0f);
                    break;
                case VIEW_LEFT:
                    newPos[2] = clamp(newPos[2], -1.0f, 1.0f);
                    newPos[1] = clamp(newPos[1], -1.0f, 1.0f);
                    break;
                case VIEW_RIGHT:
                    newPos[0] = clamp(newPos[0], -1.0f, 1.0f);
                    newPos[2] = clamp(newPos[2], -1.0f, 1.0f);
                    break;
            }

            model->updateVertex(selectedVertex, newPos[0], newPos[1], newPos[2]);
        }

    } else if (dragged) { // touch was just released
        dragged = false;
        if (selectedVertex) {
            model->generateTris();
            updateVBO(model->triArray, model->triCount);
        }
    }

    //debug info
    staticTextBuf = C2D_TextBufNew(128);
    C2D_TextParse(&txt, staticTextBuf, std::to_string(touch.px).c_str());
	C2D_TextOptimize(&txt);
}
bool SceneEditor::handleKeys(){
    hidScanInput();
    u32 kDown = hidKeysDown();
    if (kDown & KEY_START)
        return true; // break in order to return to hbmenu

    u32 kHeld = hidKeysHeld();
    if ((kHeld & KEY_DRIGHT))
        angleY += 1.0f/256;
    if ((kHeld & KEY_DLEFT))
        angleY -= 1.0f/256;
    if ((kHeld & KEY_DUP))
        angleX -= 1.0f/256;
    if ((kHeld & KEY_DDOWN))
        angleX += 1.0f/256;

    return false;
}
void SceneEditor::tick(){

}
void SceneEditor::renderTop3D(float iod){
    drawVBO(iod,modelSprite.image.tex);
}
void SceneEditor::renderTop2D(float iod){
    //C2D_DrawCircleSolid(0,0,0,10,C2D_Color32f(0.0f, 0.0f, 1.0f, 1.0f));
    //C2D_DrawSprite(&modelSprite);
}
void SceneEditor::renderBottom3D(){

}
void SceneEditor::renderBottom2D() {
    // Draw lines for all face edges
    for (const auto& face : model->faces) {
        for (size_t i = 0; i < face.size(); ++i) {
            Vertex* v1 = face[i].v;
            Vertex* v2 = face[(i + 1) % face.size()].v;
            auto [x1, y1] = projectTo2D(v1);
            auto [x2, y2] = projectTo2D(v2);

            C2D_DrawLine(x1, y1, C2D_Color32(200, 200, 200, 255),
                         x2, y2, C2D_Color32(200, 200, 200, 255), 1.5f, 0);
        }
    }

    // Draw vertex handles
    for (size_t i = 0; i < model->vertices.size(); ++i) {
        auto [x, y] = projectTo2D(&model->vertices[i]);
        C2D_DrawCircleSolid(x, y, 0, VERTEX_RADIUS, C2D_Color32(255, 100, 100, 255));
    }

    C2D_DrawText(&txt, 0, 8.0f, 8.0f, 1.0f, 1.0f, 1.0f);
}

std::pair<float, float> SceneEditor::projectTo2D(Vertex* v) {
    float modelX = 0.0f;
    float modelY = 0.0f;

    switch (state) {
        case VIEW_TOP:
            modelX = v->pos[0];
            modelY = -v->pos[1];
            break;
        case VIEW_LEFT:
            modelX = v->pos[2];
            modelY = -v->pos[1];
            break;
        case VIEW_RIGHT:
            modelX = v->pos[0];
            modelY = -v->pos[2];
            break;
    }

    // Viewport size
    float viewportWidth  = c2[0] - c1[0];
    float viewportHeight = c2[1] - c1[1];

    // Define model viewing area (example: -1 to +1 square)
    float modelSize = 2.0f; // we assume model fits in [-1,+1] range in both axes

    // Compute uniform scale factor to fit in viewport
    float scaleX = viewportWidth  / modelSize;
    float scaleY = viewportHeight / modelSize;
    float scale = std::min(scaleX, scaleY); // uniform scaling

    // Center of viewport
    float centerX = (c1[0] + c2[0]) / 2.0f;
    float centerY = (c1[1] + c2[1]) / 2.0f;

    // Apply uniform scale and center
    float screenX = modelX * scale + centerX;
    float screenY = modelY * scale + centerY;

    return {screenX, screenY};
}
std::array<float, 3> SceneEditor::screenToModel(float screenX, float screenY, const Vertex* reference) {
    float viewportWidth  = c2[0] - c1[0];
    float viewportHeight = c2[1] - c1[1];
    float modelSize = 2.0f;

    float scaleX = viewportWidth / modelSize;
    float scaleY = viewportHeight / modelSize;
    float scale = std::min(scaleX, scaleY);

    float centerX = (c1[0] + c2[0]) / 2.0f;
    float centerY = (c1[1] + c2[1]) / 2.0f;

    float modelA = (screenX - centerX) / scale;
    float modelB = (screenY - centerY) / scale;

    std::array<float, 3> result = reference ? std::array<float, 3>{
        reference->pos[0], reference->pos[1], reference->pos[2]
    } : std::array<float, 3>{0.0f, 0.0f, 0.0f};

    switch (state) {
        case VIEW_TOP:
            result[0] = modelA;     // X
            result[1] = -modelB;    // Y (flipped)
            break;
        case VIEW_LEFT:
            result[2] = modelA;     // Z
            result[1] = -modelB;    // Y
            break;
        case VIEW_RIGHT:
            result[0] = modelA;     // X
            result[2] = -modelB;    // Z
            break;
    }

    return result;
}
