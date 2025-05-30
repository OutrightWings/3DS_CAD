#pragma once
#include "scene.hpp"
class ScenePauseMenu : public Scene {
    public:
        void init(){
            C2D_SpriteFromSheet(&pauseMenuSprite, getSpriteSheet(), 6);
            C2D_SpriteSetCenter(&pauseMenuSprite, 0, 0);
            C2D_SpriteSetPos(&pauseMenuSprite, 39, 32);

            C2D_SpriteFromSheet(&vertexOptionsSprite, getSpriteSheet(), 10);
            C2D_SpriteSetCenter(&vertexOptionsSprite, 0, 0);
            C2D_SpriteSetPos(&vertexOptionsSprite, 39, 129);
        }
        bool userInput(){
            hidScanInput();
            touchPosition touch;
            hidTouchRead(&touch);
            if (!(touch.px == 0 && touch.py == 0)) { // screen is touched
                if(current_scene == scene_vertex_editor){
                    for (int i = 0; i < VERTEX_OPTIONS_BUTTON_COUNT; ++i) {
                        if (vertex_options_buttons[i]->isClicked(touch.px, touch.py)) {
                            vertex_editor_buttons[i]->isSelected = !vertex_editor_buttons[i]->isSelected;
                            break;
                        }
                    }
                }
                for (int i = 0; i < PAUSE_BUTTON_COUNT; ++i) {
                    if (pause_menu_buttons[i]->isClicked(touch.px, touch.py)) {
                        switch (i) {
                            case BUTTON_NEW:
                                
                                break;
                            case BUTTON_SAVE:
                                
                                break;
                            case BUTTON_LOAD:
                                
                                break;
                            case BUTTON_UV:
                                break;
                            case BUTTON_SPRITE:
                                break;
                            case BUTTON_VERTEX:
                                changeScene(scene_vertex_editor);
                                break;
                        }
                        break;
                    }
                }
            }
            return false;
        }
        void tick(){}
        void renderTop3D(float iod){drawVBO(iod,modelSprite.image.tex);}
        void renderBottom3D(){}
        void renderTop2D(float iod){}
        void renderBottom2D(){
            //pause
            C2D_DrawSprite(&pauseMenuSprite);
            for(Button* b: pause_menu_buttons){
                b->drawButton();
            }

            //options
            if(current_scene == scene_vertex_editor) {
                C2D_DrawSprite(&vertexOptionsSprite);
                for(Button* b: vertex_options_buttons){
                    b->drawButton();
                }
            }
        }
    private:
        C2D_Sprite pauseMenuSprite, vertexOptionsSprite;
};