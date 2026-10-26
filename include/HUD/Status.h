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

	//health tile offsets
	int healthTileOffsets[2][10] = {
		//x positions
		{ 13,14,14,14,14,14,14,14,14,16 },
		{ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 }
		//y positions
	};


	//mana tile offsets
	int manaTileOffsets[2][8] = {
		//x positions
		{ 13,14,14,14,14,14,14,16 },
		{ 8, 8, 8, 8, 8, 8, 8, 8 }
		//y positions
	};

	int emptyHealthTileOffsets[2][10] = {
		//x positions
		{ 13,14,14,14,14,14,14, 14, 14, 16 },
		{ 6,  6, 6, 6, 6, 6, 6, 6,  6,  6 }
		//y positions
	};

	int emptyManaTileOffsets[2][8] = {
		//x positions
		{ 13,14,14,14,14,14,14, 16 },
		{ 6,  6, 6, 6, 6, 6, 6, 6 }
		//y positions
	};



public:

	Status(GamesEngineeringBase::Image* _image)
	{
		image = _image;
	}

	void draw(GamesEngineeringBase::Window& window, int health, int maxHealth, int mana, int maxMana)
	{
		//position for what y level the status will sit on
		int healthYPos = 0;
		int manaYPos = 32;
		int healthStartXPos = window.getWidth();
		int manaStartXPos = window.getWidth();

		int percentageHP = maxHealth / 10; //5 tiles for health
		int healthCounter = maxHealth; //take away a percentage for the first slice


		int percentageMana = maxMana / 8; //4 tiles for mana
		int manaCounter = maxMana; //take away a percentage for the first slice

		//draw health right to left
		for (int i = 9; i >= 0; i--) //for each tile (5 tiles in total)
		{
			if (health < healthCounter) //if health is less than health counter
			{	//draw empty
				Graphics::DrawTilemapImage(window, *image, tileSize, emptyHealthTileOffsets[0][i], emptyHealthTileOffsets[1][i], healthStartXPos - tileSize, healthYPos);
			}
			else //draw health
			{
				Graphics::DrawTilemapImage(window, *image, tileSize, healthTileOffsets[0][i], healthTileOffsets[1][i], healthStartXPos - tileSize, healthYPos);

			}
			
			healthCounter -= percentageHP; //decrease health counter by maxhp / 5 , which is 1/5 as health is split into 5 segments
			healthStartXPos -= tileSize; //decrease offset pos for next tile

		}


		//draw mana right to left
		for (int i = 7; i >= 0; i--) //for each tile (4 tiles in total)
		{
			if (mana < manaCounter) //if mana is less than mana counter
			{	//draw empty
				Graphics::DrawTilemapImage(window, *image, tileSize, emptyManaTileOffsets[0][i], emptyManaTileOffsets[1][i], manaStartXPos - tileSize, manaYPos);
			}
			else //draw health
			{
				Graphics::DrawTilemapImage(window, *image, tileSize, manaTileOffsets[0][i], manaTileOffsets[1][i], manaStartXPos - tileSize, manaYPos);

			}

			manaCounter -= percentageMana; //decrease health counter by maxhp / 5 , which is 1/5 as health is split into 5 segments
			manaStartXPos -= tileSize; //decrease offset pos for next tile

		}

	}

};