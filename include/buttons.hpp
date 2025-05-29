#pragma once
#include "modelObject.hpp"
#include "scenes.hpp"
#include "spriteSheet.hpp"
#include "3dMath.hpp"
#include "graphics.hpp"

class Button{
	public: 
	Button(int _x, int _y, int _w, int _h){
		x = _x;
		y = _y;
		w = _w;
		h = _h;
	}
	int x, y, w, h;
	bool isClicked(int px, int py){
		return px >= x && px <= x+w && py >= y && py <= y+h;
	}
};
class VertexButton : Button {
    public:
    VertexButton(Vertex* _v) : Button(0,0,6,6){
        v = _v;

        C2D_SpriteFromSheet(&unselected, getSpriteSheet(), 2);
        C2D_SpriteSetCenter(&unselected, 0.5f, 0.5f);
        C2D_SpriteFromSheet(&selected, getSpriteSheet(), 3);
        C2D_SpriteSetCenter(&selected, 0.5f, 0.5f);

        updateSpritePos();
    }
    void updatePos(float screenX, float screenY){
        std::array<float, 3> result = screenToModelSpace(screenX,screenY,state,v);
        for(int i = 0; i < 3; i++){
            v->pos[i] = result[i];
        }
        updateSpritePos();
    }
    std::pair<float, float> getPos(){
        return modelToScreenSpace(state,v);
    }
    
    bool isClicked(int px, int py){
        std::pair<float, float> pos = getPos();
        float x = pos.first;
        float y = pos.second;
        return px >= x-w && px <= x+w && py >= y-h && py <= y+h; //These are centered sprites
    }
    void updateSpritePos(){
        std::pair<float, float> pos = getPos();
        float x = pos.first;
        float y = pos.second;
        C2D_SpriteSetPos(&unselected, x, y);
        C2D_SpriteSetPos(&selected, x, y);

        depth = depthValue(state,v);
        C2D_SpriteSetDepth(&unselected, depth);
        C2D_SpriteSetDepth(&selected, depth);
    }
    void drawButton(){
        updateSpritePos();
        if(isSelected){
            C2D_DrawSprite(&selected);
        } else {
            C2D_DrawSprite(&unselected);
        }
    }
    Vertex* v;
    float depth;
    C2D_Sprite unselected, selected;
    bool isSelected;
};
extern Button loadButton;
extern Button saveButton;
extern Button deleteButton;
extern Button newButton;
extern Button topButton;
extern Button leftButton;
extern Button rightButton;
extern Button topOpButton;
extern Button leftOpButton;
extern Button rightOpButton;