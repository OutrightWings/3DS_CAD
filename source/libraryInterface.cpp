#include "libraryInterface.hpp"

void initLib(){
	//Base library
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	//Screens
	createScreens();

	//Sprites
	loadSpriteSheets();
}
void deinitLib(){
	freeSpriteSheets();
	
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
}