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
    C2D_SpriteSetPos(&selectedViewSprite, TOP_BAR_TOP_X, 0);

    C2D_SpriteFromSheet(&modelSprite, getModelSheet(), 0);
    
    model = new Model();
    updateVBO(model->triArray,model->triCount);
    
    state = VIEW_TOP;

    //Create vertex buttons
    for(int i = 0; i < model->vertices.size(); i++){
        vertexButtons.push_back(VertexButton(&model->vertices[i],state));
    }

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
            if(touch.py <= TOP_BAR_BUTTON_HEIGHT){
                if(loadButton.isClicked(touch.px,touch.py)){
                    C2D_SpriteSetPos(&selectedViewSprite, 0, 0);
                }
            } else {
                // We are just starting to drag: find the vertex under the touch point
                selectedVertButton = nullptr;
                float bestDistSq = VERTEX_RADIUS * VERTEX_RADIUS;

                for (VertexButton& v : vertexButtons) {
                    std::pair<float, float> pos = v.getPos();
                    float dx = pos.first - touch.px;
                    float dy = pos.second - touch.py;
                    float distSq = dx * dx + dy * dy;

                    if (distSq <= bestDistSq) {
                        bestDistSq = distSq;
                        selectedVertButton = &v;
                        selectedVertButton->isSelected = true;
                    }
                }
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
    for (std::vector<FaceVertex> face : model->faces) {
        for (size_t i = 0; i < face.size(); ++i) {
            Vertex* v1 = face[i].v;
            Vertex* v2 = face[(i + 1) % face.size()].v;
            std::pair<float, float> pair1 = modelToScreenSpace(state,v1);
            std::pair<float, float> pair2 = modelToScreenSpace(state,v2);

            C2D_DrawLine(pair1.first, pair1.second, C2D_Color32(200, 200, 200, 255),
                         pair2.first, pair2.second, C2D_Color32(200, 200, 200, 255), 1.5f, 0);
        }
    }

    // Draw vertex handles
    for(int i = 0; i < vertexButtons.size(); i++){
        vertexButtons[i].drawButton();
    }

    //C2D_DrawText(&txt, 0, 8.0f, 8.0f, 1.0f, 1.0f, 1.0f);
    C2D_DrawSprite(&topBarSprite);
    C2D_DrawSprite(&selectedViewSprite);
}