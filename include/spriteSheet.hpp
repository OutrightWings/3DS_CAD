#pragma once
#include <citro2d.h>
#include <3ds.h>
#include "constants.hpp"

void loadSpriteSheets();
void freeSpriteSheets();
C2D_SpriteSheet getSpriteSheet();
C2D_SpriteSheet getModelSheet();
void initSprites();