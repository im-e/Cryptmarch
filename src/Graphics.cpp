#include "Graphics.h"
void Graphics::DrawImage(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, int startX, int startY)
{
	//convert image width and height from unsigned int to int for calculations
	int imageWidth = image.width;
	int imageHeight = image.height;
	int windowWidth = window.getWidth();
	int windowHeight = window.getHeight();

	//check if the image is completely outside the window boundaries
	if (startX >= windowWidth ||
		startY >= windowHeight ||
		startX + imageWidth <= 0 ||
		startY + imageHeight <= 0)
	{
		return; // image is completely outside the window, no need to render
	}


	int renderWidth = imageWidth; //render width used for clipping calculations
	int renderHeight = imageHeight; 
	int screenX = 0;
	int screenY = 0;

	//clip left
	if (startX < 0)
	{
		screenX = -startX; // set screenX to the amount clipped
		renderWidth += startX; // reduce width by the amount clipped
		startX = 0; // set startX to 0
	}
	
	//clip right
	if (startX + renderWidth > windowWidth)
	{
		renderWidth = windowWidth - startX; // reduce width by the amount clipped
	}

	//clip top
	if (startY < 0)
	{
		screenY = -startY; // set screenY to the amount clipped
		renderHeight += startY; // reduce height by the amount clipped
		startY = 0; // set startY to 0
	}

	//clip bottom
	if (startY + renderHeight > windowHeight)
	{
		renderHeight = windowHeight - startY; // reduce height by the amount clipped
	}

	for (int y = 0; y < renderHeight; y++)
	{
		for (int x = 0; x < renderWidth; x++)
		{
			unsigned char alpha = image.alphaAtUnchecked(screenX + x, screenY + y); //check alpha value of the pixel 

			if (alpha == 255) //if the alpha is fully opaque, draw the pixel to the window
			{
				unsigned char* pixel = image.atUnchecked(screenX + x, screenY + y); //get the pixel data from the image
				window.draw(startX + x, startY + y, pixel[0], pixel[1], pixel[2]); //draw the pixel to the window
			}
			else if (alpha > 0) //if the alpha is partially transparent, blend the pixel with the window pixel
			{
				unsigned char* pixel = image.at(screenX + x, screenY + y); //get the pixel data from the image

				int index = (startY + y) * windowWidth + (startX + x); //calculate the index of the pixel in the window back buffer

				unsigned char* windowPixel = window.getBackBuffer() + index * 3; //get the pixel data from the window
				//window back buffer is a flat array of unsigned char, where each pixel is represented by 3 bytes (RGB), so we multiply the index by 3 to get the correct position in the back buffer

				//blend the pixel with the window pixel using alpha blending formula
				windowPixel[0] = (pixel[0] * alpha + windowPixel[0] * (255 - alpha)) / 255;
				windowPixel[1] = (pixel[1] * alpha + windowPixel[1] * (255 - alpha)) / 255;
				windowPixel[2] = (pixel[2] * alpha + windowPixel[2] * (255 - alpha)) / 255;

				window.draw(startX + x, startY + y, windowPixel[0], windowPixel[1], windowPixel[2]); //draw the blended pixel to the window
			}
			//ignore pixels with alpha = 0 (fully transparent)
		}
	}
}
void Graphics::DrawTilemapImage(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, int tileSize, int tilemapOffsetX, int tilemapOffsetY, int startX, int startY)
{
	//convert image width and height from unsigned int to int for calculations

	int imageWidth = image.width;
	int imageHeight = image.height;
	int windowWidth = window.getWidth();
	int windowHeight = window.getHeight();

	//check if the image is completely outside the window boundaries
	if (startX >= windowWidth ||
		startY >= windowHeight ||
		startX + tileSize <= 0 || //using tile size instead of image size
		startY + tileSize <= 0)
	{
		return; // image is completely outside the window, no need to render
	}


	int renderWidth = tileSize; //render width used for clipping calculations
	int renderHeight = tileSize; //using tile size instead of image size
	int screenX = 0;
	int screenY = 0;

	//clip left
	if (startX < 0)
	{
		screenX = -startX; // set screenX to the amount clipped
		renderWidth += startX; // reduce width by the amount clipped
		startX = 0; // set startX to 0
	}

	//clip right
	if (startX + renderWidth > windowWidth)
	{
		renderWidth = windowWidth - startX; // reduce width by the amount clipped
	}

	//clip top
	if (startY < 0)
	{
		screenY = -startY; // set screenY to the amount clipped
		renderHeight += startY; // reduce height by the amount clipped
		startY = 0; // set startY to 0
	}

	//clip bottom
	if (startY + renderHeight > windowHeight)
	{
		renderHeight = windowHeight - startY; // reduce height by the amount clipped
	}

	int tileX = tilemapOffsetX * tileSize; //use the offset eg: [3,5] and multiply it by tile size
	int tileY = tilemapOffsetY * tileSize;	//to get the tile position 

	//i had to swap y to the outside and x on the inside to ensure we go row by row
	//this matters because when the cpu reads a byte it fetches a cache line (64 bytes) and keeps it, so reading close by data is faster
	for (int y = 0; y < renderHeight; y++)
	{
		for (int x = 0; x < renderWidth; x++)
		{
			//can use at unchecked due to clipping code
			unsigned char alpha = image.alphaAtUnchecked(screenX + x + tileX, screenY + y + tileY); //check alpha value of the pixel with the tile offset 

			if (alpha == 255) //if the alpha is fully opaque, draw the pixel to the window
			{
				unsigned char* pixel = image.atUnchecked(tileX + screenX + x, tileY + screenY + y); //get the pixel data from the image with the tile offset
				window.draw(startX + x, startY + y, pixel[0], pixel[1], pixel[2]); //draw the pixel to the window
			}
			else if (alpha > 0) //if the alpha is partially transparent, blend the pixel with the window pixel
			{
				unsigned char* pixel = image.atUnchecked(tileX + screenX + x, tileY + screenY + y); //get the pixel data from the image with the tile offset

				int index = (startY + y) * windowWidth + (startX + x); //calculate the index of the pixel in the window back buffer

				unsigned char* windowPixel = window.getBackBuffer() + index * 3; //get the pixel data from the window
				//window back buffer is a flat array of unsigned char, where each pixel is represented by 3 bytes (RGB), so we multiply the index by 3 to get the correct position in the back buffer

				//blend the pixel with the window pixel using alpha blending formula
				windowPixel[0] = (pixel[0] * alpha + windowPixel[0] * (255 - alpha)) / 255;
				windowPixel[1] = (pixel[1] * alpha + windowPixel[1] * (255 - alpha)) / 255;
				windowPixel[2] = (pixel[2] * alpha + windowPixel[2] * (255 - alpha)) / 255;

				window.draw(startX + x, startY + y, windowPixel[0], windowPixel[1], windowPixel[2]); //draw the blended pixel to the window
			}
			//ignore pixels with alpha = 0 (fully transparent)
		}
	}
}