#include "spriteSheet.hpp"
C2D_SpriteSheet spriteSheet;
C2D_SpriteSheet modelSheet;
C2D_SpriteSheet getSpriteSheet(){
    return spriteSheet;
}
C2D_SpriteSheet getModelSheet(){
	return modelSheet;
}
void loadSpriteSheets(){
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	modelSheet = C2D_SpriteSheetLoad("romfs:/gfx/model.t3x");

	if (!modelSheet || !spriteSheet) svcBreak(USERBREAK_PANIC);
}
void freeSpriteSheets(){
	C2D_SpriteSheetFree(spriteSheet);
	C2D_SpriteSheetFree(modelSheet);
}