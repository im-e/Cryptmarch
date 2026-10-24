#pragma once
#include "GamesEngineeringBase.h"
#include "Rendering/Graphics.h"
#include <iostream>


class FPS
{

private:

	//image
	GamesEngineeringBase::Image* image;
	int tileSize = 32;

	//fps calculation
	int frames = 0;
	double timeElapsed = 0.0;
	double fps = 0.0;

	//fps tile offsets
	int fpsNumTileOffsets[2][10] = {
		//x positions
		{ 3,4,5,6,7,8,9,10,11,12 },
		{ 5,5,5,5,5,5,5,5,5,5 }
		//y positions
	};

	int fpsTextTileOffsets[2][3] = {
		//x positions
		{ 5,2, 5 },
		{ 9,10,10 }
		//y positions
	};

public:

	FPS(GamesEngineeringBase::Image* _image)
	{
		image = _image;
	}

	double calculate(float dt)
	{

		frames++; //increase frame amoumt
		timeElapsed += dt; //add the time elapsed

		if (timeElapsed > 0.1) //if time is been 0.1s
		{
			fps = frames / timeElapsed; //divide the frames by the time elapsed
			frames = 0; //reset for next calculation
			timeElapsed = 0; //reset time elapsed

		}

		return fps; //return fps

	}

	void draw(GamesEngineeringBase::Window& window, float dt)
	{
		//position for what y level the fps will sit on
		int fpsYPos = 0;

		//draw fps label left to right
		int labelStartPos = 0;
		for (int i = 0; i < 3; i++) //for each character
		{
			Graphics::DrawTilemapImage(window, *image, tileSize, fpsTextTileOffsets[0][i], fpsTextTileOffsets[1][i], labelStartPos, fpsYPos); //draw character on screen
			labelStartPos += tileSize; //increase offset pos for next character

		}


		//get fps
		int value = static_cast<int>(calculate(dt)); //use static cast for c++ explicit conversion
	
		//visually displayed like - FPS3000
		//32 px per tile 32f * 7 = 224
		int finalDigitXPos = 224; //set position for final digit pos

		if (value == 0) //if value equal to zeo
		{
			Graphics::DrawTilemapImage(window, *image, tileSize, fpsNumTileOffsets[0][0], fpsNumTileOffsets[1][0], finalDigitXPos, fpsYPos); //draw zeo
		}
		
		while (value > 0)
		{
			int digit = value % 10; //get the final digit of the number
			value /= 10; // divide value by 10
			Graphics::DrawTilemapImage(window, *image, tileSize, fpsNumTileOffsets[0][digit], fpsNumTileOffsets[1][digit], finalDigitXPos, fpsYPos); //draw digit on screen

			finalDigitXPos -= tileSize; // step left for the next digit
		}

	}
	
	double get() const 
	{ 
		return fps; 
	}



};