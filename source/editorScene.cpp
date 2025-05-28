#include "scenes.hpp"
#include "spriteSheet.hpp"
#include "graphics.hpp"
#include "buttons.hpp"
#include <string>


static C2D_TextBuf staticTextBuf;
static C2D_Text txt;


static bool dragged = false;

std::vector<VertexButton> vertexButtons;
VertexButton* selectedVertButton = nullptr;

const float VERTEX_RADIUS = 6.0f;

void SceneEditor::init(){
    C2D_SpriteFromSheet(&topBarSprite, getSpriteSheet(), 0);
    C2D_SpriteSetCenter(&topBarSprite, 0, 0);
    C2D_SpriteSetPos(&topBarSprite, 0, 0);

    C2D_SpriteFromSheet(&selectedViewSprite, getSpriteSheet(), 1);
    C2D_SpriteSetCenter(&selectedViewSprite, 0, 0);
    C2D_SpriteSetPos(&selectedViewSprite, topButton.x, topButton.y);

    state = VIEW_TOP;

    //Create vertex buttons
    for(size_t i = 0; i < model->vertices.size(); i++){
        vertexButtons.push_back(VertexButton(&model->vertices[i]));
    }

    // staticTextBuf = C2D_TextBufNew(128);
	// C2D_TextParse(&txt, staticTextBuf, std::to_string(model->triCount).c_str());
	// C2D_TextOptimize(&txt);
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
            if(touch.py <= TOP_BAR_BUTTON_HEIGHT){
                //Check Buttons for click
                if(loadButton.isClicked(touch.px,touch.py)){
                    //TODO
                } 
                else if(saveButton.isClicked(touch.px,touch.py)){
                    //TODO
                }
                else if(deleteButton.isClicked(touch.px,touch.py)){
                    //TODO
                }
                else if(newButton.isClicked(touch.px,touch.py)){
                    //TODO
                }
                else if(topButton.isClicked(touch.px,touch.py)){
                    C2D_SpriteSetPos(&selectedViewSprite, topButton.x, topButton.y);
                    state = state == VIEW_TOP ? VIEW_OPP_TOP : VIEW_TOP;
                    presetRotate(state);
                }
                else if(leftButton.isClicked(touch.px,touch.py)){
                    C2D_SpriteSetPos(&selectedViewSprite, leftButton.x, leftButton.y);
                    state = state == VIEW_LEFT ? VIEW_OPP_LEFT : VIEW_LEFT;
                    presetRotate(state);
                }
                else if(rightButton.isClicked(touch.px,touch.py)){
                    C2D_SpriteSetPos(&selectedViewSprite, rightButton.x, rightButton.y);
                    state = state == VIEW_RIGHT ? VIEW_OPP_RIGHT : VIEW_RIGHT;
                    presetRotate(state);
                }
            } else {
                // We are just starting to drag: find the vertex under the touch point
                selectedVertButton = nullptr;
                float bestDistSq = VERTEX_RADIUS * VERTEX_RADIUS;
                float depth = -1;
                for (VertexButton& v : vertexButtons) {
                    std::pair<float, float> pos = v.getPos();
                    float dx = pos.first - touch.px;
                    float dy = pos.second - touch.py;
                    float distSq = dx * dx + dy * dy;

                    if (distSq <= bestDistSq && v.depth >= depth) {
                        bestDistSq = distSq;
                        selectedVertButton = &v;
                        depth = v.depth;
                    }
                }
                if(selectedVertButton != nullptr)
                    selectedVertButton->isSelected = true;
            }
        }
        else if (selectedVertButton && dragged) { //Move vertex with stylus
            //Update vertex
            selectedVertButton->updatePos(touch.px, touch.py);
            model->updateVertex(selectedVertButton->v);
        }
        dragged = true;

    } else if (dragged) { // touch was just released
        dragged = false;
        if (selectedVertButton) {
            model->generateTris();
            updateVBO(model->triArray, model->triCount);
            selectedVertButton->isSelected = false;
            selectedVertButton = nullptr;
        }
    }

    //debug info
    // staticTextBuf = C2D_TextBufNew(128);
    // float biggest = 0, smallest = 10;
    // for(Vertex& v : model->vertices){
    //     if(biggest < v.pos[0]){
    //         biggest = v.pos[0];
    //     }
    //     if(smallest > v.pos[0]){
    //         smallest = v.pos[0];
    //     }
    // }
    // C2D_TextParse(&txt, staticTextBuf, (std::to_string(biggest) + " " + std::to_string(smallest)).c_str());
	// C2D_TextOptimize(&txt);
}
bool SceneEditor::handleKeys(){
    hidScanInput();
    u32 kDown = hidKeysDown();
    if (kDown & KEY_START)
        return true; // break in order to return to hbmenu

    u32 key = hidKeysHeld();
    if ((key & KEY_DRIGHT) || (key & KEY_DLEFT) || (key & KEY_DUP) || (key & KEY_DDOWN))
        rotate(key);

    return false;
}
void SceneEditor::tick(){

}
void SceneEditor::renderTop3D(float iod){
    drawVBO(iod,modelSprite.image.tex);
}
void SceneEditor::renderTop2D(float iod){
    //C2D_DrawText(&txt, 0, 8.0f, 8.0f, 1.0f, 0.5f, 0.5f);
}
void SceneEditor::renderBottom3D(){

}
void SceneEditor::renderBottom2D() {
    // Draw lines for all face edges
    for (std::vector<FaceVertex> face : model->faces) {
        for (size_t i = 0; i < face.size(); ++i) {
            Vertex* v1 = face[i].v;
            Vertex* v2 = face[(i + 1) % face.size()].v;
            std::pair<float, float> pair1 = modelToScreenSpace(state,v1);
            std::pair<float, float> pair2 = modelToScreenSpace(state,v2);
            float d1 = depthValue(state, v1);
            float d2 = depthValue(state, v2);
            C2D_DrawLine(pair1.first, pair1.second, depthColor(d1),
                         pair2.first, pair2.second, depthColor(d2), 1.5f, (d1 + d2) /2.0f);
        }
    }

    // Draw vertex handles
    for(size_t i = 0; i < vertexButtons.size(); i++){
        vertexButtons[i].drawButton();
    }

    C2D_DrawSprite(&topBarSprite);
    C2D_DrawSprite(&selectedViewSprite);
}