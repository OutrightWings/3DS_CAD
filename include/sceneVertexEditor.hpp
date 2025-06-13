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

            C2D_DrawLine(xMin,yMin,CLOSE_LINE_COLOR,xMax,yMax,CLOSE_LINE_COLOR,3,1);
        }
    private:
        C2D_TextBuf staticTextBuf;
        C2D_Text txt;

        std::vector<VertexButton> vertexButtons;
        std::vector<VertexButton*> selectedVertButtons;
        
        float VERTEX_RADIUS = 6.0f;

        touchPosition touchPos;
        int startX, startY;
        int xMin, xMax, yMin, yMax;

        bool dragging = false;
        bool overVertex = false;
        bool multi_select = false;

        C2D_Sprite editorBarSprite, vertexAreaSprite;

        void handleTouch(){
            hidTouchRead(&touchPos);
            //is screen touched
            if(!(touchPos.px == 0 && touchPos.py == 0)){
                if(dragging){
                    //If we are on a point, we should be moving it and all selected by a delta
                    if(!multi_select){
                        //Get delta and set new start to here
                        float dx = touchPos.px - startX;
                        float dy = touchPos.py - startY;
                        startX = touchPos.px;
                        startY = touchPos.py;
                        for(VertexButton *b : selectedVertButtons){
                            b->updatePos(dx,dy);
                        }
                    }
                    //If we are not on a point be saving corners for the drag area
                    else if(multi_select){
                        updateDragArea(touchPos);
                    }
                }
                else{
                    //Did we just click a button
                    if(touchPos.px >= EDITOR_BAR_X)
                        handleButtons();
                    else {
                        dragging = true;
                        //Start dragging area
                        startX = touchPos.px;
                        startY = touchPos.py;
                        updateDragArea(touchPos);
                        //did we click whitespace
                        std::vector<VertexButton*> under = getNearestVertexHandles(false);
                        if(!multi_select){
                            //Under is empty
                            if(under.empty()){
                                //deselect everything prior
                                clickAllButtons(selectedVertButtons);
                                selectedVertButtons.clear();
                            }
                            //Under is not in our list of selected nodes so replace it
                            else if(!under.at(0)->isSelected){
                                //deselect everything prior
                                clickAllButtons(selectedVertButtons);
                                selectedVertButtons.clear();
                                //Update with new
                                selectedVertButtons = under;
                                clickAllButtons(selectedVertButtons);
                            }
                        }
                        
                    }
                }
            }
            else if(dragging){
                //update the vbo with the points we moved
                model->generateTris();
                updateVBO(model->triArray,model->triCount);
                //if multi select nodes/release nodes
                if(multi_select){
                    std::vector<VertexButton*> under = getNearestVertexHandles(true);
                    
                    //toggle state of buttons that are under.
                    clickAllButtons(under);
                    //merge lists
                    if(!selectedVertButtons.empty()){
                        // for(VertexButton *b : under){
                        //     auto pos = std::find(selectedVertButtons.begin(),selectedVertButtons.end(),b);
                        //     //not in the list add
                        //     if( pos != selectedVertButtons.end()){
                        //         selectedVertButtons.push_back(b);
                        //     }
                        //     else { //in list remove
                        //         selectedVertButtons.erase(pos);
                        //     }
                        // }
                    }
                    //make new list
                    else{
                        for(VertexButton *b : under){
                            if(b->isSelected)
                                selectedVertButtons.push_back(b);
                        }
                    }
                    
                }
                //clean up
                dragging = false;
                overVertex = false;
                startX = 0;
                startY = 0;
                updateDragArea(touchPos);
            }
        }
        void clickAllButtons(std::vector<VertexButton*> buttons){
            for(VertexButton* b : buttons){
                b->click();
            }
        }

        void updateDragArea(touchPosition end){
            xMax = fmax(startX,end.px);
            xMin = fmin(startX,end.px);

            yMax = fmax(startY,end.py);
            yMin = fmin(startY,end.py);
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
            //todo check other buttons
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

        std::vector<VertexButton*> getNearestVertexHandles(bool multi){
            consoleClear();
            printf("\x1b[1;1H%03d-%03d",xMin,xMax);
            printf("\x1b[2;1H%03d-%03d\n",yMin,yMax);

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
                    if( pos.first >= xMin -VERTEX_RADIUS && 
                        pos.first <= xMax  +VERTEX_RADIUS&&
                        pos.second >= yMin  -VERTEX_RADIUS&&
                        pos.second <= yMax  +VERTEX_RADIUS
                        ){
                        printf("%04f %04f\n",pos.first,pos.second);
                        list.push_back(&v);
                    }
                }
            }

            if(!multi && temp)
                list.push_back(temp);
            return list;
        }
};