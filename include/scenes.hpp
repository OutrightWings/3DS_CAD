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
        virtual Model* getModel();
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
        Model* getModel(){
            return model;
        }
    private:
        C2D_Sprite topBarSprite, selectedViewSprite;
        C2D_Sprite modelSprite;
        Model *model;
        ViewState state;
        void handleTouch();
        bool handleKeys();
};