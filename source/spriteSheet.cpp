#include "spriteSheet.hpp"
C2D_SpriteSheet spriteSheet;
C2D_SpriteSheet getSpriteSheet(){
    return spriteSheet;
}
void loadSpriteSheets(){
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);
}
void freeSpriteSheets(){
	C2D_SpriteSheetFree(spriteSheet);
}