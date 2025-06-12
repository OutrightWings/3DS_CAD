#pragma once
#include "modelObject.hpp"
#include "scene.hpp"
#include "spriteSheet.hpp"
#include "3dMath.hpp"
#include "graphics.hpp"

class Button{
	public:
    C2D_Sprite unselected, selected;
    bool isSelected = false;
    int idUn, idSel;
	Button(int _x, int _y, int _w, int _h, int unselectedSpriteID, int selectedSpriteID){
		x = _x;
		y = _y;
		w = _w;
		h = _h;
        
        idUn = unselectedSpriteID;
        idSel = selectedSpriteID;

        if(unselectedSpriteID != -1){
            C2D_SpriteFromSheet(&unselected, getSpriteSheet(), unselectedSpriteID);
            C2D_SpriteSetCenter(&unselected, 0, 0);
            C2D_SpriteSetPos(&unselected, x, y);
            C2D_SpriteSetDepth(&unselected, 0);
        }
        if(selectedSpriteID != -1){
            C2D_SpriteFromSheet(&selected, getSpriteSheet(), selectedSpriteID);
            C2D_SpriteSetCenter(&selected, 0, 0);
            C2D_SpriteSetPos(&selected, x, y);
            C2D_SpriteSetDepth(&selected, 0);
        }
            
	}
	int x, y, w, h;
	bool isClicked(int px, int py){
		return px >= x && px <= x+w && py >= y && py <= y+h;
	}
    virtual void drawButton(){
        if(isSelected && idSel != -1){
            C2D_DrawSprite(&selected);
        } else if(!isSelected && idUn != -1){
            C2D_DrawSprite(&unselected);
        }
    }
};
class ViewButton : public Button {
    public:
        ViewButton(int _x, int _y, int _w, int _h, int unselectedSpriteID, int selectedSpriteID, ViewState _s) : Button(_x,_y,_w,_h,unselectedSpriteID,selectedSpriteID){
            state = _s;
        }
        void click(){
            presetRotate(state);
        }
    private:
        ViewState state;
};
class VertexButton : public Button {
    public:
    VertexButton(Vertex* _v) : Button(0,0,6,6, 2, 3){
        v = _v;

        C2D_SpriteSetCenter(&unselected, 0.5f, 0.5f);
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
    void drawButton() override{
        updateSpritePos();
        Button::drawButton();
    }
    Vertex* v;
    float depth;
};

enum VertexEditorButtonID {
    BUTTON_DELETE_VERTEX,
    BUTTON_NEW_VERTEX,
    BUTTON_TOP,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_OPP_TOP,
    BUTTON_OPP_LEFT,
    BUTTON_OPP_RIGHT,
    VERTEX_BUTTON_COUNT
};
enum PauseMenuButtonID {
    BUTTON_SAVE,
    BUTTON_LOAD,
    BUTTON_NEW,
    BUTTON_UV,
    BUTTON_SPRITE,
    BUTTON_VERTEX,
    PAUSE_BUTTON_COUNT
};
enum VertexOptionsButtonID {
    BUTTON_GRID_SNAP,
    BUTTON_DEPTH_COLOR,
    VERTEX_OPTIONS_BUTTON_COUNT
};
void initButtons();
extern std::vector<Button *> vertex_editor_buttons;
extern std::vector<Button *> pause_menu_buttons;
extern std::vector<Button *> vertex_options_buttons;