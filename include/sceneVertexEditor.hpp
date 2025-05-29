#pragma once
#include "scene.hpp"

#include <citro2d.h>
#include <3ds.h>
#include "modelObject.hpp"
#include "3dMath.hpp"
#include "spriteSheet.hpp"
#include "graphics.hpp"
#include "buttons.hpp"
#include <string>


class SceneVertexEditor :public Scene {
    public:
        void init(){
            C2D_SpriteFromSheet(&editorBarSprite, getSpriteSheet(), 0);
            C2D_SpriteSetCenter(&editorBarSprite, 0, 0);
            C2D_SpriteSetPos(&editorBarSprite, 276, 0);
            //C2D_SpriteSetDepth(&editorBarSprite,100);

            C2D_SpriteFromSheet(&selectedViewSprite, getSpriteSheet(), 1);
            C2D_SpriteSetCenter(&selectedViewSprite, 0, 0);
            C2D_SpriteSetPos(&selectedViewSprite, topButton.x, topButton.y);
            //C2D_SpriteSetDepth(&selectedViewSprite,101);

            C2D_SpriteFromSheet(&vertexAreaSprite, getSpriteSheet(), 5);
            C2D_SpriteSetCenter(&vertexAreaSprite, 0, 0);
            C2D_SpriteSetPos(&vertexAreaSprite, PRO_C1_X, PRO_C1_Y);
            C2D_SpriteSetDepth(&vertexAreaSprite,-1);

            state = VIEW_TOP;

            //Create vertex buttons
            for(size_t i = 0; i < model->vertices.size(); i++){
                vertexButtons.push_back(VertexButton(&model->vertices[i]));
            }

            // staticTextBuf = C2D_TextBufNew(128);
            // C2D_TextParse(&txt, staticTextBuf, std::to_string(model->triCount).c_str());
            // C2D_TextOptimize(&txt);
        }
        bool userInput(){
            bool exit;
            exit = handleKeys();
            handleTouch();

            return exit;
        }
        void tick(){}
        void renderTop3D(float iod){
            drawVBO(iod,modelSprite.image.tex);
        }
        void renderBottom3D(){}
        void renderTop2D(float iod){}
        void renderBottom2D() {
            C2D_DrawSprite(&vertexAreaSprite);
            
            // Draw lines for all face edges
            for (std::vector<FaceVertex> face : model->faces) {
                for (size_t i = 0; i < face.size(); ++i) {
                    Vertex* v1 = face[i].v;
                    Vertex* v2 = face[(i + 1) % face.size()].v;
                    std::pair<float, float> pair1 = modelToScreenSpace(state,v1);
                    std::pair<float, float> pair2 = modelToScreenSpace(state,v2);
                    float d1 = depthValue(state, v1);
                    float d2 = depthValue(state, v2);
                    C2D_DrawLine(pair1.first, pair1.second, depthColor(d1),
                                pair2.first, pair2.second, depthColor(d2), 3, (d1 + d2) /2.0f);
                }
            }

            // Draw vertex handles
            for(size_t i = 0; i < vertexButtons.size(); i++){
                vertexButtons[i].drawButton();
            }

            C2D_DrawSprite(&editorBarSprite);
            C2D_DrawSprite(&selectedViewSprite);
        }
    private:
        //C2D_TextBuf staticTextBuf;
        //C2D_Text txt;

        std::vector<VertexButton> vertexButtons;
        VertexButton* selectedVertButton = nullptr;
        
        float VERTEX_RADIUS = 6.0f;

        bool dragged = false;
        C2D_Sprite editorBarSprite, selectedViewSprite, vertexAreaSprite;

        void handleTouch(){
            touchPosition touch;
            hidTouchRead(&touch);

            if (!(touch.px == 0 && touch.py == 0)) { // screen is touched
                if (!dragged) {
                    if(touch.px >= EDITOR_BAR_X){
                        //Check Buttons for click
                        if(loadButton.isClicked(touch.px,touch.py)){
                            //TODO
                        } 
                        else if(saveButton.isClicked(touch.px,touch.py)){
                            //TODO
                        }
                        else if(deleteButton.isClicked(touch.px,touch.py)){
                            //TODO
                        }
                        else if(newButton.isClicked(touch.px,touch.py)){
                            //TODO
                        }
                        else if(topButton.isClicked(touch.px,touch.py)){
                            C2D_SpriteSetPos(&selectedViewSprite, topButton.x, topButton.y);
                            state = VIEW_TOP;
                            presetRotate(state);
                        }
                        else if(leftButton.isClicked(touch.px,touch.py)){
                            C2D_SpriteSetPos(&selectedViewSprite, leftButton.x, leftButton.y);
                            state = VIEW_LEFT;
                            presetRotate(state);
                        }
                        else if(rightButton.isClicked(touch.px,touch.py)){
                            C2D_SpriteSetPos(&selectedViewSprite, rightButton.x, rightButton.y);
                            state = VIEW_RIGHT;
                            presetRotate(state);
                        }
                        else if(rightOpButton.isClicked(touch.px,touch.py)){
                            C2D_SpriteSetPos(&selectedViewSprite, rightOpButton.x, rightOpButton.y);
                            state = VIEW_OPP_RIGHT;
                            presetRotate(state);
                        }
                        else if(leftOpButton.isClicked(touch.px,touch.py)){
                            C2D_SpriteSetPos(&selectedViewSprite, leftOpButton.x, leftOpButton.y);
                            state = VIEW_OPP_LEFT;
                            presetRotate(state);
                        }
                        else if(topOpButton.isClicked(touch.px,touch.py)){
                            C2D_SpriteSetPos(&selectedViewSprite, topOpButton.x, topOpButton.y);
                            state = VIEW_OPP_TOP;
                            presetRotate(state);
                        }
                    } else {
                        // We are just starting to drag: find the vertex under the touch point
                        selectedVertButton = nullptr;
                        float bestDistSq = VERTEX_RADIUS * VERTEX_RADIUS;
                        float depth = -1;
                        for (VertexButton& v : vertexButtons) {
                            std::pair<float, float> pos = v.getPos();
                            float dx = pos.first - touch.px;
                            float dy = pos.second - touch.py;
                            float distSq = dx * dx + dy * dy;

                            if (distSq <= bestDistSq && v.depth >= depth) {
                                bestDistSq = distSq;
                                selectedVertButton = &v;
                                depth = v.depth;
                            }
                        }
                        if(selectedVertButton != nullptr)
                            selectedVertButton->isSelected = true;
                    }
                }
                else if (selectedVertButton && dragged) { //Move vertex with stylus
                    //Update vertex
                    selectedVertButton->updatePos(touch.px, touch.py);
                    model->updateVertex(selectedVertButton->v);
                }
                dragged = true;

            } else if (dragged) { // touch was just released
                dragged = false;
                if (selectedVertButton) {
                    model->generateTris();
                    updateVBO(model->triArray, model->triCount);
                    selectedVertButton->isSelected = false;
                    selectedVertButton = nullptr;
                }
            }

            //debug info
            // staticTextBuf = C2D_TextBufNew(128);
            // float biggest = 0, smallest = 10;
            // for(Vertex& v : model->vertices){
            //     if(biggest < v.pos[0]){
            //         biggest = v.pos[0];
            //     }
            //     if(smallest > v.pos[0]){
            //         smallest = v.pos[0];
            //     }
            // }
            // C2D_TextParse(&txt, staticTextBuf, (std::to_string(biggest) + " " + std::to_string(smallest)).c_str());
            // C2D_TextOptimize(&txt);
        }
        bool handleKeys(){
            hidScanInput();
            u32 kDown = hidKeysDown();
            if (kDown & KEY_START)
                return true; // break in order to return to hbmenu

            u32 key = hidKeysHeld();
            if ((key & KEY_DRIGHT) || (key & KEY_DLEFT) || (key & KEY_DUP) || (key & KEY_DDOWN))
                rotate(key);

            return false;
        }
};