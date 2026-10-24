#pragma once
#include "GamesEngineeringBase.h"
class Graphics
{
public:
	static const int tileSize = 32;

	static void DrawImage(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, int posX, int posY);
	static void DrawTilemapImage(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, int tileSize, int tilemapOffsetX, int tileOffsetY, int posX, int posY);
	static void DrawRectOutline(GamesEngineeringBase::Window& window,int left, int top, int width, int height, unsigned char r, unsigned char g, unsigned char b);
	static void DrawLetter(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, char letter, int posX, int posY);
	static void DrawNumber(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, int value, int posX, int posY);
	static void DrawString(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, const char* text, int posX, int posY);
	//had to call it DrawString from DrawText because of another definition overlap
};