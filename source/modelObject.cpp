#include "modelObject.hpp"

Model* model;
C2D_Sprite modelSprite;

void initModel(){
    model = new Model();
    C2D_SpriteFromSheet(&modelSprite, getModelSheet(), 0);
}