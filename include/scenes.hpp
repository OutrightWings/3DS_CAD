#pragma once
#include <citro2d.h>
#include <3ds.h>
#include "modelObject.hpp"

template<typename T>
T clamp(T value, T min, T max) {
    return (value < min) ? min : (value > max) ? max : value;
}

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
        C2D_Sprite viewSprite;
        C2D_Sprite modelSprite;
        Model *model;
        enum ViewState { VIEW_TOP, VIEW_LEFT, VIEW_RIGHT };
        ViewState state;
        std::pair<float, float> projectTo2D(Vertex* v);
        std::array<float, 3> screenToModel(float screenX, float screenY, const Vertex* reference);
        void handleTouch();
        bool handleKeys();
};