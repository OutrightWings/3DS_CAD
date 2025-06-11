#pragma once
#include "spriteSheet.hpp"
class Scene {
    public:
        virtual void init(){
            C2D_SpriteFromSheet(&bottomBackgroundSprite, getSpriteSheet(), 11);
            C2D_SpriteSetCenter(&bottomBackgroundSprite, 0, 0);
            C2D_SpriteSetPos(&bottomBackgroundSprite, 0, 0);
            C2D_SpriteSetDepth(&bottomBackgroundSprite,-1);
        }
        virtual bool userInput();
        virtual void tick();
        virtual void renderTop3D(float iod);
        virtual void renderBottom3D();
        virtual void renderTop2D(float iod);
        virtual void renderBottom2D(){
            C2D_DrawSprite(&bottomBackgroundSprite);
        }
    protected:
        C2D_Sprite bottomBackgroundSprite;
};

extern Scene* current_scene;
extern Scene *scene_vertex_editor, *scene_pause_menu, *scene_start_menu;

void changeScene(Scene* scene);
void initScenes();