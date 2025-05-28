#pragma once
#include <citro2d.h>
#include <citro3d.h>
#include <3ds.h>
#include "vshader_shbin.h"
#include "constants.hpp"
#include "scenes.hpp"
#include "modelObject.hpp"
#include "3dMath.hpp"

extern float angleX;
extern float angleY;
extern ViewState state;

void drawVBO(float iod,C3D_Tex *tex);
void sceneBind(C3D_Tex *tex);
void initShader();
void deinitShader();
void renderPipeline(Scene*);
void createScreens();
void updateVBO(Vertex* list, int count);