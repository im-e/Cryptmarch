#include "Rendering/Graphics.h"
#include "Utilities/Logger.h"
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

	int renderWidth = imageWidth; //used for clipping calculations
	int renderHeight = imageHeight; 
	int clippedX = 0;
	int clippedY = 0;

	//clip left
	if (startX < 0) //if startX is in the negative (offscreen)
	{
		clippedX = -startX; // set to the positive version of amount clipped
		renderWidth += startX; // reduce render width by the amount clipped as we will have to render less of the image
		startX = 0; // set startX to 0 so that render starts inside the window
	}
	
	//clip right
	if (startX + renderWidth > windowWidth) //if image would go off the right of the screen
	{
		renderWidth = windowWidth - startX; // reduce width by the amount that is offscreen
	}

	//clip top
	if (startY < 0) //if startY is in the negative (offscreen)
	{
		clippedY = -startY; // set to the positive version of amount clipped
		renderHeight += startY; // reduce render height by the amount clipped as we will have to render less of the image
		startY = 0; // set startY to 0 so that render starts inside the window
	}

	//clip bottom
	if (startY + renderHeight > windowHeight) //if image would go off the bottom of the screen
	{
		renderHeight = windowHeight - startY; // reduce height by the amount clipped
	}

	int channels = image.channels; //find the number of channels of the image
	if(channels != 4) //Channels of image need to be 4 for following code to work
	{
		Logger::Error("Channels != 4");
		return;
	}

	for (int y = 0; y < renderHeight; y++)
	{
		//for each row, work out the positions once
		unsigned char* imagePixel = image.atUnchecked(clippedX, clippedY + y); //gets the pixel of the image in the first index of the row
		int index = ((startY + y) * windowWidth + startX) * 3; //calculate the index of the pixel in the window back buffer (window is RGB (only 3 channels))
		unsigned char* windowPixel = window.getBackBuffer() + index; //uses the index to get the pixel of the window of where we want to draw

		for (int x = 0; x < renderWidth; x++)
		{
			unsigned char alpha = imagePixel[3]; //get the alpha value (A)

			if (alpha == 255) //if max alpha (opaque)
			{
				windowPixel[0] = imagePixel[0]; //write the image data directly to the pixel via pointer
				windowPixel[1] = imagePixel[1];
				windowPixel[2] = imagePixel[2];
			}
			else if (alpha > 0) //partly transparent
			{	//apply pixel blending formula
				windowPixel[0] = (imagePixel[0] * alpha + windowPixel[0] * (255 - alpha)) / 255; //write image data to the pixel via pointer
				windowPixel[1] = (imagePixel[1] * alpha + windowPixel[1] * (255 - alpha)) / 255;
				windowPixel[2] = (imagePixel[2] * alpha + windowPixel[2] * (255 - alpha)) / 255;
			}
			//ignore pixels with alpha = 0 (fully transparent)

			imagePixel += channels; //increase the index of our image pixel by channels to get to the next pixel
			windowPixel += 3; //same applies to the window, however we add 3 due to window being RGB not RGBA
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
		startX + tileSize <= 0 ||
		startY + tileSize <= 0)
	{
		return; // image is completely outside the window, no need to render
	}

	int renderWidth = tileSize; //used for clipping calculations
	int renderHeight = tileSize;
	int clippedX = 0;
	int clippedY = 0;

	//clip left
	if (startX < 0) //if startX is in the negative (offscreen)
	{
		clippedX = -startX; // set to the positive version of amount clipped
		renderWidth += startX; // reduce render width by the amount clipped as we will have to render less of the image
		startX = 0; // set startX to 0 so that render starts inside the window
	}

	//clip right
	if (startX + renderWidth > windowWidth) //if image would go off the right of the screen
	{
		renderWidth = windowWidth - startX; // reduce width by the amount that is offscreen
	}

	//clip top
	if (startY < 0) //if startY is in the negative (offscreen)
	{
		clippedY = -startY; // set to the positive version of amount clipped
		renderHeight += startY; // reduce render height by the amount clipped as we will have to render less of the image
		startY = 0; // set startY to 0 so that render starts inside the window
	}

	//clip bottom
	if (startY + renderHeight > windowHeight) //if image would go off the bottom of the screen
	{
		renderHeight = windowHeight - startY; // reduce height by the amount clipped
	}

	int tileX = tilemapOffsetX * tileSize; //use the offset eg: [3,5] and multiply it by tile size
	int tileY = tilemapOffsetY * tileSize;	//to get the tile position 

	
	int channels = image.channels; //find the number of channels of the image
	if (channels != 4) //Channels of image need to be 4 for following code to work
	{
		Logger::Error("Channels != 4");
		return;
	}

	for (int y = 0; y < renderHeight; y++)
	{
		//for each row, work out the positions once
		unsigned char* imagePixel = image.atUnchecked(tileX + clippedX, tileY + clippedY + y); //gets the pixel of the image in the first index of the row
		int index = ((startY + y) * windowWidth + startX) * 3; //calculate the index of the pixel in the window back buffer (window is RGB (only 3 channels))
		unsigned char* windowPixel = window.getBackBuffer() + index; //uses the index to get the pixel of the window of where we want to draw

		for (int x = 0; x < renderWidth; x++)
		{
			unsigned char alpha = imagePixel[3]; //get the alpha value (A)

			if (alpha == 255) //if max alpha (opaque)
			{
				windowPixel[0] = imagePixel[0]; //write the image data directly to the pixel via pointer
				windowPixel[1] = imagePixel[1];
				windowPixel[2] = imagePixel[2];
			}
			else if (alpha > 0) //partly transparent
			{	//apply pixel blending formula
				windowPixel[0] = (imagePixel[0] * alpha + windowPixel[0] * (255 - alpha)) / 255; //write image data to the pixel via pointer
				windowPixel[1] = (imagePixel[1] * alpha + windowPixel[1] * (255 - alpha)) / 255;
				windowPixel[2] = (imagePixel[2] * alpha + windowPixel[2] * (255 - alpha)) / 255;
			}
			//ignore pixels with alpha = 0 (fully transparent)

			imagePixel += channels; //increase the index of our image pixel by channels to get to the next pixel
			windowPixel += 3; //same applies to the window, however we add 3 due to window being RGB not RGBA
		}
	}

}

void Graphics::DrawRectOutline(GamesEngineeringBase::Window& window, int left, int top, int width, int height, unsigned char r, unsigned char g, unsigned char b)
{
	if (width <= 0 || height <= 0) return; //nothing to draw

	//get window height and width
	int windowWidth = window.getWidth();
	int windowHeight = window.getHeight();
	//get right of the rect
	int right = left + width - 1;   //-1 for pixel index
	int bottom = top + height - 1; 

	//completely outside the window
	if (right < 0 || 
		bottom < 0 ||
		left >= windowWidth || 
		top >= windowHeight)
		return;

	//clamp the span of each edge to the window
	int drawStartX;
	if (left < 0) //if the left edge is off the left of the screen
	{
		drawStartX = 0; //start drawing from the edge of the window instead
	}
	else
	{
		drawStartX = left;
	}

	int drawEndX;
	if (right >= windowWidth) //if the right edge is off the right of the screen
	{
		drawEndX = windowWidth - 1; //stop drawing at the last pixel of the window
	}
	else
	{
		drawEndX = right;
	}

	int drawStartY;
	if (top < 0) //if the top edge is off the top of the screen
	{
		drawStartY = 0;
	}
	else
	{
		drawStartY = top;
	}

	int drawEndY;
	if (bottom >= windowHeight) //if the bottom edge is off the bottom of the screen
	{
		drawEndY = windowHeight - 1;
	}
	else
	{
		drawEndY = bottom;
	}

	//only draw an edge if that edge's line is actually on screen,
	//otherwise clamping would draw a fake edge along the window border
	if (top >= 0) //top line
	{
		for (int x = drawStartX; x <= drawEndX; x++)
		{
			window.draw(x, top, r, g, b);
		}
	}

	if (bottom < windowHeight) //bottom line
	{
		for (int x = drawStartX; x <= drawEndX; x++)
		{
			window.draw(x, bottom, r, g, b);
		}
	}

	if (left >= 0) //left line
	{
		for (int y = drawStartY; y <= drawEndY; y++)
		{
			window.draw(left, y, r, g, b);
		}
	}

	if (right < windowWidth) //right line
	{
		for (int y = drawStartY; y <= drawEndY; y++)
		{
			window.draw(right, y, r, g, b);
		}
	}
}