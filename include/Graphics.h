#pragma once
#include "GamesEngineeringBase.h"
class Graphics
{
public:
	static void DrawImage(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, int startX, int startY);
	static void DrawTilemapImage(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, int tileSize, int tilemapOffsetX, int tileOffsetY, int startX, int startY);

};