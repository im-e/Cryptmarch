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

	for (int i = 0; i < renderWidth; i++)
	{
		for (int j = 0; j < renderHeight; j++)
		{
			unsigned char alpha = image.alphaAt(screenX + i, screenY + j); //check alpha value of the pixel 

			if (alpha == 255) //if the alpha is fully opaque, draw the pixel to the window
			{
				unsigned char* pixel = image.at(screenX + i, screenY + j); //get the pixel data from the image
				window.draw(startX + i, startY + j, pixel[0], pixel[1], pixel[2]); //draw the pixel to the window
			}
			else if (alpha > 0) //if the alpha is partially transparent, blend the pixel with the window pixel
			{
				unsigned char* pixel = image.at(screenX + i, screenY + j); //get the pixel data from the image

				int index = (startY + j) * windowWidth + (startX + i); //calculate the index of the pixel in the window back buffer

				unsigned char* windowPixel = window.getBackBuffer() + index * 3; //get the pixel data from the window
				//window back buffer is a flat array of unsigned char, where each pixel is represented by 3 bytes (RGB), so we multiply the index by 3 to get the correct position in the back buffer

				//blend the pixel with the window pixel using alpha blending formula
				windowPixel[0] = (pixel[0] * alpha + windowPixel[0] * (255 - alpha)) / 255;
				windowPixel[1] = (pixel[1] * alpha + windowPixel[1] * (255 - alpha)) / 255;
				windowPixel[2] = (pixel[2] * alpha + windowPixel[2] * (255 - alpha)) / 255;

				window.draw(startX + i, startY + j, windowPixel[0], windowPixel[1], windowPixel[2]); //draw the blended pixel to the window
			}
			//ignore pixels with alpha = 0 (fully transparent)
		}
	}
}