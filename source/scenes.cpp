#include "scene.hpp"

Scene* current_scene;

void changeScene(Scene* scene){
    scene->init();
    current_scene = scene;
}