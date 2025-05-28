#include "3dMath.hpp"

float yaw = 0, pitch = 0;
C3D_Mtx cameraRotation;

template<typename T>
T clamp(T value, T min, T max) {
    return (value < min) ? min : (value > max) ? max : value;
}

std::array<float, 3> screenToModelSpace(float screenX, float screenY, ViewState s, Vertex* v){
    float viewportWidth  = PRO_C2_X - PRO_C1_X;
    float viewportHeight = PRO_C2_Y - PRO_C1_Y;
    float modelSize = 2.0f;

    float scaleX = viewportWidth / modelSize;
    float scaleY = viewportHeight / modelSize;
    float scale = std::min(scaleX, scaleY);

    float centerX = (PRO_C2_X + PRO_C1_X) / 2.0f;
    float centerY = (PRO_C2_Y + PRO_C1_Y) / 2.0f;

    float modelA = (screenX - centerX) / scale;
    float modelB = (screenY - centerY) / scale;

    std::array<float, 3> result = v ? std::array<float, 3>{
        v->pos[0], v->pos[1], v->pos[2]
    } : std::array<float, 3>{0.0f, 0.0f, 0.0f};

    switch (s) {
        case VIEW_TOP:
            result[0] = modelA;     // X
            result[1] = -modelB;    // Y (flipped)
            break;
        case VIEW_LEFT:
            result[2] = modelA;     // Z
            result[1] = -modelB;    // Y
            break;
        case VIEW_RIGHT:
            result[0] = modelA;     // X
            result[2] = -modelB;    // Z
            break;
            case VIEW_OPP_TOP:
            result[0] = -modelA;     // X
            result[1] = -modelB;    // Y (flipped)
            break;
        case VIEW_OPP_LEFT:
            result[2] = -modelA;     // Z
            result[1] = -modelB;    // Y
            break;
        case VIEW_OPP_RIGHT:
            result[0] = -modelA;     // X
            result[2] = -modelB;    // Z
            break;
    }
    result[0] = clamp(result[0], -1.0f, 1.0f);
    result[1] = clamp(result[1], -1.0f, 1.0f);
    result[2] = clamp(result[2], -1.0f, 1.0f);
    return result;
}
std::pair<float, float> modelToScreenSpace(ViewState s, Vertex* v){
    float modelX = 0.0f;
    float modelY = 0.0f;

    switch (s) {
        case VIEW_TOP:
            modelX = v->pos[0];
            modelY = -v->pos[1];
            break;
        case VIEW_LEFT:
            modelX = v->pos[2];
            modelY = -v->pos[1];
            break;
        case VIEW_RIGHT:
            modelX = v->pos[0];
            modelY = -v->pos[2];
            break;
        case VIEW_OPP_TOP:
            modelX = -v->pos[0];
            modelY = -v->pos[1];
            break;
        case VIEW_OPP_LEFT:
            modelX = -v->pos[2];
            modelY = -v->pos[1];
            break;
        case VIEW_OPP_RIGHT:
            modelX = -v->pos[0];
            modelY = -v->pos[2];
            break;
    }

    // Viewport size
    float viewportWidth  = PRO_C2_X - PRO_C1_X;
    float viewportHeight = PRO_C2_Y - PRO_C1_Y;

    // Define model viewing area (example: -1 to +1 square)
    float modelSize = 2.0f; // we assume model fits in [-1,+1] range in both axes

    // Compute uniform scale factor to fit in viewport
    float scaleX = viewportWidth  / modelSize;
    float scaleY = viewportHeight / modelSize;
    float scale = std::min(scaleX, scaleY); // uniform scaling

    // Center of viewport
    float centerX = (PRO_C2_X + PRO_C1_X) / 2.0f;
    float centerY = (PRO_C2_Y + PRO_C1_Y) / 2.0f;

    // Apply uniform scale and center
    float screenX = modelX * scale + centerX;
    float screenY = modelY * scale + centerY;

    return {screenX, screenY};
}
float depthValue(ViewState s, Vertex* v){
    float depth = 0;
    switch (s) {
        case VIEW_TOP:
            depth = v->pos[2];
            break;
        case VIEW_LEFT:
            depth = -v->pos[0];
            break;
        case VIEW_RIGHT:
            depth = -v->pos[1];
            break;
        case VIEW_OPP_TOP:
            depth = -v->pos[2];
            break;
        case VIEW_OPP_LEFT:
            depth = v->pos[0];
            break;
        case VIEW_OPP_RIGHT:
            depth = v->pos[1];
            break;
    }
    return depth;
}
u32 depthColor(float depth){
    //normalize from range [-1, 1]
    float max = 1, min = -1;
    float nm = (depth - min) / (max - min);

    u8 out[4];
    for(int i = 0; i < 4; i++){
        u8 f = (FAR_LINE_COLOR >> (24 - i * 8)) & 0xff;
        u8 n = (CLOSE_LINE_COLOR >> (24 - i * 8)) & 0xff;
        out[i] = f + (n - f) * nm;
    }
    return C2D_Color32(out[0],out[1],out[2],out[3]);
}
void rotate(u32 kHeld){
    const float sensitivity = C3D_AngleFromDegrees(1.0f); // 1 degree per frame

    C3D_Mtx rotation;
    Mtx_Identity(&rotation);

    // Pitch (local X axis)
    if (kHeld & KEY_DDOWN) {
        Mtx_RotateX(&rotation, -sensitivity, true);
        Mtx_Multiply(&cameraRotation, &cameraRotation, &rotation);
    }
    if (kHeld & KEY_DUP) {
        Mtx_RotateX(&rotation, sensitivity, true);
        Mtx_Multiply(&cameraRotation, &cameraRotation, &rotation);
    }

    // Yaw (local Y axis)
    if (kHeld & KEY_DLEFT) {
        Mtx_RotateY(&rotation, sensitivity, true);
        Mtx_Multiply(&cameraRotation, &cameraRotation, &rotation);
    }
    if (kHeld & KEY_DRIGHT) {
        Mtx_RotateY(&rotation, -sensitivity, true);
        Mtx_Multiply(&cameraRotation, &cameraRotation, &rotation);
    }
}
void presetRotate(ViewState view){
    Mtx_Identity(&cameraRotation);
    switch(view){
        case VIEW_LEFT:
            Mtx_RotateY(&cameraRotation, C3D_AngleFromDegrees(-90), false);
            break;
        case VIEW_RIGHT:
            Mtx_RotateX(&cameraRotation, C3D_AngleFromDegrees(90), false);
            break;
        case VIEW_TOP:
            
            break;
        case VIEW_OPP_TOP:
            Mtx_RotateY(&cameraRotation, C3D_AngleFromDegrees(180), false);
            break;
        case VIEW_OPP_LEFT:
            Mtx_RotateY(&cameraRotation, C3D_AngleFromDegrees(90), false);
            break;
        case VIEW_OPP_RIGHT:
            Mtx_RotateX(&cameraRotation, C3D_AngleFromDegrees(-90), false);
            Mtx_RotateY(&cameraRotation, C3D_AngleFromDegrees(180), false);
            break;
    }
}