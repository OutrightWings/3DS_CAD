#pragma once
#include <citro3d.h>
#include "constants.hpp"
#include <algorithm>
#include "modelObject.hpp"

extern C3D_Mtx cameraRotation;


enum ViewState { VIEW_TOP, VIEW_LEFT, VIEW_RIGHT, VIEW_OPP_TOP, VIEW_OPP_LEFT, VIEW_OPP_RIGHT };
template<typename T>
T clamp(T value, T min, T max);
std::array<float, 3> screenToModelSpace(float screenX, float screenY, ViewState s, Vertex* v);
std::pair<float, float> modelToScreenSpace(ViewState s, Vertex* v);
float depthValue(ViewState s, Vertex* v);
u32 depthColor(float p);
void rotate(u32 direction, float ratio);
void presetRotate(ViewState view);