#include "libraryInterface.hpp"
#include "buttons.hpp"
void initLib(){
	//Base library
	romfsInit();
	gfxInitDefault();
	gfxSet3D(true);
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);

	//Screens
	createScreens();

	//Sprites
	loadSpriteSheets();
	
	initModel();
	initShader();

	initScenes();
	initButtons();
}
void deinitLib(){
	freeSpriteSheets();
	deinitShader();
	C3D_Fini();
	gfxExit();
	romfsExit();
}