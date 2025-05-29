#pragma once
#include <citro2d.h>
#include <3ds.h>
#include "modelObject.hpp"
#include "3dMath.hpp"

class Scene {
    public:
        virtual void init();
        virtual bool userInput();
        virtual void tick();
        virtual void renderTop3D(float iod);
        virtual void renderBottom3D();
        virtual void renderTop2D(float iod);
        virtual void renderBottom2D();
};
class SceneEditor :public Scene {
    public:
        void init();
        bool userInput();
        void tick();
        void renderTop3D(float iod);
        void renderBottom3D();
        void renderTop2D(float iod);
        void renderBottom2D();
    private:
        C2D_Sprite editorBarSprite, selectedViewSprite, vertexAreaSprite;
        void handleTouch();
        bool handleKeys();
};
class StartMenu : public Scene {
    public:
        void init();
        bool userInput();
        void tick();
        void renderTop3D(float iod);
        void renderBottom3D();
        void renderTop2D(float iod);
        void renderBottom2D();
    private:
        C2D_Sprite logoSprite;
};

extern Scene* current_scene;
void changeScene(Scene* scene);