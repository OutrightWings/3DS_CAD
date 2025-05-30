#pragma once
#include <citro2d.h>
#include "modelObject.hpp"
#include "spriteSheet.hpp"
#include "graphics.hpp"

#include "scene.hpp"

class SceneStartMenu : public Scene {
    public:
        void init(){
            C2D_SpriteFromSheet(&logoSprite, getSpriteSheet(), 4);
            C2D_SpriteSetCenter(&logoSprite, 0.5f, 1);
            C2D_SpriteSetPos(&logoSprite, SCREEN_WIDTH/2, SCREEN_HEIGHT);
        }
        bool userInput(){
            hidScanInput();
            touchPosition touch;
            hidTouchRead(&touch);
            
            if (!(touch.px == 0 && touch.py == 0)) { // screen is touched
                changeScene(scene_pause_menu);
            }
            
            u32 kDown = hidKeysDown();
            if (kDown & KEY_START)
                return true; // break in order to return to hbmenu
            return false;
        }
        void tick(){}
        void renderTop3D(float iod){
            drawVBO(iod,modelSprite.image.tex);
        }
        void renderBottom3D(){}
        void renderTop2D(float iod){
            C2D_DrawSprite(&logoSprite);
        }
        void renderBottom2D(){}
    private:
        C2D_Sprite logoSprite;
};