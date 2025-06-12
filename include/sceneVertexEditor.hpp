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
#include <algorithm>

class SceneVertexEditor :public Scene {
    public:
        void init(){
            Scene::init();
            C2D_SpriteFromSheet(&editorBarSprite, getSpriteSheet(), 0);
            C2D_SpriteSetCenter(&editorBarSprite, 0, 0);
            C2D_SpriteSetPos(&editorBarSprite, 276, 0);
            //C2D_SpriteSetDepth(&editorBarSprite,100);

            C2D_SpriteFromSheet(&vertexAreaSprite, getSpriteSheet(), 5);
            C2D_SpriteSetCenter(&vertexAreaSprite, 0, 0);
            C2D_SpriteSetPos(&vertexAreaSprite, PRO_C1_X, PRO_C1_Y);
            C2D_SpriteSetDepth(&vertexAreaSprite,-1);

            state = VIEW_TOP;
            vertex_editor_buttons[BUTTON_TOP]->isSelected = true;

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
        void renderTop2D(float iod){
            C2D_DrawText(&txt, 0, 8.0f, 8.0f, 1.0f, 0.5f, 0.5f);
        }
        void renderBottom2D() {
            Scene::renderBottom2D();
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

            // Draw vertex handles class VertexButton
            for(size_t i = 0; i < vertexButtons.size(); i++){
                vertexButtons[i].drawButton();
            }

            C2D_DrawSprite(&editorBarSprite);
            for (Button* b : vertex_editor_buttons) {
                b->drawButton();
            }

            float xMin, xMax, yMin, yMax;
            xMin = fmin(dragStartPos.px,touchPos.px);
            yMin = fmin(dragStartPos.py,touchPos.py);
            xMax = fmax(dragStartPos.px,touchPos.px);
            yMax = fmax(dragStartPos.py,touchPos.py);
            C2D_DrawLine(xMin,yMin,CLOSE_LINE_COLOR,xMax,yMax,CLOSE_LINE_COLOR,3,1);
        }
    private:
        C2D_TextBuf staticTextBuf;
        C2D_Text txt;

        std::vector<VertexButton> vertexButtons;
        std::vector<VertexButton*> selectedVertButtons;
        
        float VERTEX_RADIUS = 6.0f;

        touchPosition dragStartPos, touchPos;
        int xMin, xMax, yMin, yMax;

        bool dragging = false;
        bool multi_select = false;

        C2D_Sprite editorBarSprite, vertexAreaSprite;

        void handleTouch(){
            hidTouchRead(&touchPos);
            
            if (!(touchPos.px == 0 && touchPos.py == 0)) { // screen is touched
                if (!dragging) { //Not already dragging
                    if(touchPos.px >= EDITOR_BAR_X){ //Clicked in the button area
                        handleButtons();

                    } else { // We are just starting to drag
                        
                        dragStartPos = touchPos;

                        // if(!selectedVertButtons.empty()){
                        //     for(VertexButton *b : selectedVertButtons){
                        //         b->isSelected = false;
                        //     }
                        //     selectedVertButtons.clear();
                        // }
                        
                        // std::vector<VertexButton*> temp = getNearestVertexHandle();

                        // if(temp != nullptr){
                        //     temp->isSelected = true;
                        //     selectedVertButtons.push_back(temp);
                        // }
                    }
                }
                else if (!selectedVertButtons.empty() && dragging) { //Move vertices with stylus
                    //todo
                    for(VertexButton *b : selectedVertButtons){
                        b->updatePos(touchPos.px, touchPos.py);
                        model->updateVertex(b->v);
                    }
                    
                }
                dragging = true;

            } else if (dragging) { // touch was just released
                dragging = false;
                
                //deselect everything prior
                if(!selectedVertButtons.empty()){
                    for(VertexButton *b : selectedVertButtons){
                        b->isSelected = false;
                    }
                    selectedVertButtons.clear();
                }
                //Select new things
                
                xMin = fmin(dragStartPos.px,touchPos.px);
                yMin = fmin(dragStartPos.py,touchPos.py);
                xMax = fmax(dragStartPos.px,touchPos.px);
                yMax = fmax(dragStartPos.py,touchPos.py);

                selectedVertButtons = getNearestVertexHandles(multi_select, xMin,xMax,yMin,yMax);
                for(VertexButton *b : selectedVertButtons){
                    b->isSelected = true;
                }
                dragStartPos = {0,0};
            }

            //debug info
            staticTextBuf = C2D_TextBufNew(128);
            C2D_TextParse(&txt, staticTextBuf, (std::to_string(xMin) + ", " + std::to_string(yMin) +"  "+ std::to_string(xMax) + ", " + std::to_string(yMax)).c_str());
            C2D_TextOptimize(&txt);
        }
        void handleButtons(){
            //Check View Buttons for click
            for (int i = BUTTON_TOP; i < VERTEX_BUTTON_COUNT; ++i) {
                if (vertex_editor_buttons[i]->isClicked(touchPos.px, touchPos.py)) {
                    vertex_editor_buttons[i]->isSelected = true;

                    ((ViewButton*)vertex_editor_buttons[i])->click();

                    // Deselect others:
                    for (int j = BUTTON_TOP; j < VERTEX_BUTTON_COUNT; ++j)
                        if (j != i) vertex_editor_buttons[j]->isSelected = false;
                    
                    break;
                }
            }
        }
        bool handleKeys(){
            hidScanInput();

            u32 kDown = hidKeysDown();
            if (kDown & KEY_START)
                return true; // break in order to return to hbmenu

            u32 key = hidKeysHeld();
            handleCirclePad(key);    

            if(key & KEY_DDOWN){
                multi_select = true;
            } else {
                multi_select = false;
            }

            return false;
        }
        void handleCirclePad(u32 key){
            circlePosition pos;
            hidCircleRead(&pos);
            
            //Calculate how far we are to the edge of the cicle pad max edge 1 is max 0 is center
            float x2 = pos.dx * pos.dx;
            float y2 = pos.dy * pos.dy;

            float sum = x2 + y2;

            float ratio = sum/CIRCLE_PAD_MAX;
            ratio = ratio > 1 ? 1 : ratio; // Make sure to cap it at one 
            
            rotate(key,ratio);
        }

        std::vector<VertexButton*> getNearestVertexHandles(bool multi, int xMin, int xMax, int yMin, int yMax){
            std::vector<VertexButton*> list;
            VertexButton* temp = nullptr;
            float bestDistSq = VERTEX_RADIUS * VERTEX_RADIUS;
            float depth = -1;

            for (VertexButton& v : vertexButtons) {
                std::pair<float, float> pos = v.getPos();
                //Only return 1 point that is highest
                if(!multi){
                    float dx = pos.first - xMax;
                    float dy = pos.second - yMax;
                    float distSq = dx * dx + dy * dy;

                    if (distSq <= bestDistSq && v.depth >= depth) {
                        bestDistSq = distSq;
                        temp = &v;
                        depth = v.depth;
                    }
                }
                //Return everything between the two corners
                else {
                    if( pos.first >= xMin  && 
                        pos.first <= xMax  &&
                        pos.second >= yMin  &&
                        pos.second <= yMax  
                        ){
                        list.push_back(&v);
                    }
                }
            }

            if(!multi)
                list.push_back(temp);
            return list;
        }
};