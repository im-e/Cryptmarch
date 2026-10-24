#pragma once
#include "GamesEngineeringBase.h"
#include "Rendering/Graphics.h"
#include <iostream>


class Status
{

private:

	//image
	GamesEngineeringBase::Image* image;
	int tileSize = 32;

	//fps tile offsets
	int healthTileOffsets[2][5] = {
		//x positions
		{ 13,14,14,14,16 },
		{ 7, 7, 7, 7, 7 }
		//y positions
	};

	int emptyTileOffsets[2][5] = {
		//x positions
		{ 13,14, 14, 14, 16 },
		{ 6,  6, 6,  6,  6 }
		//y positions
	};



public:

	Status(GamesEngineeringBase::Image* _image)
	{
		image = _image;
	}

	void draw(GamesEngineeringBase::Window& window, int health)
	{
		//position for what y level the status will sit on
		int healthYPos = 0;
		int healthStartXPos = window.getWidth();

		//draw health right to left

		int healthCounter = 100;

		for (int i = 4; i >= 0; i--) //for each character
		{
			if (health < healthCounter) //if health is less than health counter
			{	//draw empty
				Graphics::DrawTilemapImage(window, *image, tileSize, emptyTileOffsets[0][i], emptyTileOffsets[1][i], healthStartXPos - tileSize, healthYPos);
			}
			else //draw health
			{
				Graphics::DrawTilemapImage(window, *image, tileSize, healthTileOffsets[0][i], healthTileOffsets[1][i], healthStartXPos - tileSize, healthYPos);

			}
			
			healthCounter -= 20; //decrease health counter by 20, which is 1/5 as health is split into 5 segments
			healthStartXPos -= tileSize; //decrease offset pos for next character

		}

	}

};