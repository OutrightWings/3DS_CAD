#include "3dMath.hpp"

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

            result[0] = clamp(result[0], -1.0f, 1.0f);
            result[1] = clamp(result[1], -1.0f, 1.0f);
            break;
        case VIEW_LEFT:
            result[2] = modelA;     // Z
            result[1] = -modelB;    // Y

            result[2] = clamp(result[2], -1.0f, 1.0f);
            result[1] = clamp(result[1], -1.0f, 1.0f);
            break;
        case VIEW_RIGHT:
            result[0] = modelA;     // X
            result[2] = -modelB;    // Z

            result[0] = clamp(result[0], -1.0f, 1.0f);
            result[2] = clamp(result[2], -1.0f, 1.0f);
            break;
    }
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