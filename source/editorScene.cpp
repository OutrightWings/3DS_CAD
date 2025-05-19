#include "scenes.hpp"
#include "spriteSheet.hpp"
#include "graphics.hpp"
void SceneEditor::init(){
    C2D_SpriteFromSheet(&viewSprite, getSpriteSheet(), 0);
    C2D_SpriteSetCenter(&viewSprite, 0, 0);
    C2D_SpriteSetPos(&viewSprite, 0, 0);
}
bool SceneEditor::userInput(){
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
void  SceneEditor::tick(){

}
void SceneEditor::renderTop3D(float iod){
    drawVBO(iod);
}
void  SceneEditor::renderTop2D(float iod){
    C2D_DrawCircleSolid(0,0,0,10,C2D_Color32f(0.0f, 0.0f, 1.0f, 1.0f));
    C2D_DrawSprite(&viewSprite);
}
void SceneEditor::renderBottom3D(){

}
void  SceneEditor::renderBottom2D(){
    C2D_DrawCircleSolid(0,0,0,10,C2D_Color32f(1.0f, 0.0f, 0.0f, 1.0f));
    C2D_DrawSprite(&viewSprite);
}