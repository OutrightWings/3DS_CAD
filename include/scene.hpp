#pragma once

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

extern Scene* current_scene;
void changeScene(Scene* scene);