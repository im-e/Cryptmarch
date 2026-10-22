#pragma once
#include "GamesEngineeringBase.h"
class Graphics
{
public:
	static void DrawImage(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, int posX, int posY);
	static void DrawTilemapImage(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, int tileSize, int tilemapOffsetX, int tileOffsetY, int posX, int posY);
	static void DrawRectOutline(GamesEngineeringBase::Window& window,int left, int top, int width, int height, unsigned char r, unsigned char g, unsigned char b);
};