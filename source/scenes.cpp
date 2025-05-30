#include "scene.hpp"
#include "sceneVertexEditor.hpp"
#include "scenePauseMenu.hpp"
#include "sceneStartMenu.hpp"
Scene* current_scene;
Scene* scene_vertex_editor, *scene_pause_menu, *scene_start_menu;
void initScenes(){
    scene_pause_menu = new ScenePauseMenu();
    scene_vertex_editor = new SceneVertexEditor();
    scene_start_menu = new SceneStartMenu();
}
void changeScene(Scene* scene){
    scene->init();
    current_scene = scene;
}