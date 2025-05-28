#include "scenes.hpp"
#include "graphics.hpp"
#include "constants.hpp"

void StartMenu::init(){
    C2D_SpriteFromSheet(&logoSprite, getSpriteSheet(), 4);
    C2D_SpriteSetCenter(&logoSprite, 0.5f, 1);
    C2D_SpriteSetPos(&logoSprite, SCREEN_WIDTH/2, SCREEN_HEIGHT);
}
bool StartMenu::userInput(){
    hidScanInput();
    touchPosition touch;
    hidTouchRead(&touch);
    
    if (!(touch.px == 0 && touch.py == 0)) { // screen is touched
        changeScene(new SceneEditor());
    }
    
    u32 kDown = hidKeysDown();
     if (kDown & KEY_START)
        return true; // break in order to return to hbmenu
    return false;
}
void StartMenu::tick(){

}
void StartMenu::renderTop3D(float iod){
    drawVBO(iod,modelSprite.image.tex);
}
void StartMenu::renderTop2D(float iod){
    C2D_DrawSprite(&logoSprite);
}
void StartMenu::renderBottom3D(){

}
void StartMenu::renderBottom2D() {

}