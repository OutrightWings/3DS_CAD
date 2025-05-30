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
extern Scene *scene_vertex_editor, *scene_pause_menu, *scene_start_menu;

void changeScene(Scene* scene);
void initScenes();