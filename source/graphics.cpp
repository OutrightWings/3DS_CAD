#include "graphics.hpp"

C3D_RenderTarget* top;

void renderPipeline(Scene *current_scene){
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(top);
        current_scene->renderTop2D();
	C3D_FrameEnd(0);
}
void createScreens(){
    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
}