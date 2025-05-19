#pragma once
#include <citro2d.h>
#include <citro3d.h>
#include <3ds.h>
#include "vshader_shbin.h"
#include "constants.hpp"
#include "scenes.hpp"
#include "object.hpp"


extern float angleX;
extern float angleY;

void drawVBO(float iod,C3D_Tex *tex);
void sceneBind(C3D_Tex *tex);
void initShader();
void deinitShader();
void renderPipeline(Scene*);
void createScreens();
void updateVBO();