#include "scenes.hpp"
#include "spriteSheet.hpp"

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
    if ((kHeld & KEY_UP))
        ;
    if ((kHeld & KEY_DOWN))
        ;
    return false;
}
void  SceneEditor::tick(){

}
void SceneEditor::renderTop3D(){

}
void SceneEditor::renderBottom3D(){

}
void  SceneEditor::renderTop2D(){
    C2D_DrawCircleSolid(0,0,0,10,C2D_Color32f(0.0f, 0.0f, 1.0f, 1.0f));
    C2D_DrawSprite(&viewSprite);
}
void  SceneEditor::renderBottom2D(){
    C2D_DrawCircleSolid(0,0,0,10,C2D_Color32f(1.0f, 0.0f, 0.0f, 1.0f));
    C2D_DrawSprite(&viewSprite);
}