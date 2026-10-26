#include "Rendering/Graphics.h"
#include "Utilities/Logger.h"
void Graphics::DrawImage(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, int startX, int startY,
	unsigned char tintR, unsigned char tintG, unsigned char tintB, unsigned char tintStrength)
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

			if (alpha > 0) //if alpha is greater than 0
			{
				int r = imagePixel[0]; //split the image pixels into rgb for tint
				int g = imagePixel[1];
				int b = imagePixel[2];

				if (tintStrength > 0) //if we have applied a tint
				{
					//newColour = original + (tintColour - original) * strength
					r = r + ((tintR - r) * tintStrength) / 255; //add the tint 
					g = g + ((tintG - g) * tintStrength) / 255; //a red channel of 200 with a blue tint of 80 at 40% → 200 + (80 − 200) × 0.4 = 200 − 48 = 152.
					b = b + ((tintB - b) * tintStrength) / 255;
				}

				if (alpha == 255) //if max alpha (opaque)
				{
					windowPixel[0] = r; //write the image data directly to the pixel via pointer
					windowPixel[1] = g; //using rgb now for applied tint
					windowPixel[2] = b;
				}
				else if (alpha > 0) //partly transparent
				{	//apply pixel blending formula
					windowPixel[0] = (r * alpha + windowPixel[0] * (255 - alpha)) / 255; //write image data to the pixel via pointer
					windowPixel[1] = (g * alpha + windowPixel[1] * (255 - alpha)) / 255;
					windowPixel[2] = (b * alpha + windowPixel[2] * (255 - alpha)) / 255;
				}
				//ignore pixels with alpha = 0 (fully transparent)
			}

			imagePixel += channels; //increase the index of our image pixel by channels to get to the next pixel
			windowPixel += 3; //same applies to the window, however we add 3 due to window being RGB not RGBA
		}
	}
}
void Graphics::DrawTilemapImage(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, int tileSize, int tilemapOffsetX, int tilemapOffsetY, int startX, int startY,
	unsigned char tintR, unsigned char tintG, unsigned char tintB, unsigned char tintStrength)
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

			if (alpha > 0) //if alpha is greater than 0
			{
				int r = imagePixel[0]; //split the image pixels into rgb for tint
				int g = imagePixel[1];
				int b = imagePixel[2];

				if (tintStrength > 0) //if we have applied a tint
				{
					//newColour = original + (tintColour - original) * strength
					r = r + ((tintR - r) * tintStrength) / 255; //add the tint 
					g = g + ((tintG - g) * tintStrength) / 255; //a red channel of 200 with a blue tint of 80 at 40% → 200 + (80 − 200) × 0.4 = 200 − 48 = 152.
					b = b + ((tintB - b) * tintStrength) / 255;
				}

				if (alpha == 255) //if max alpha (opaque)
				{
					windowPixel[0] = r; //write the image data directly to the pixel via pointer
					windowPixel[1] = g; //using rgb now for applied tint
					windowPixel[2] = b;
				}
				else if (alpha > 0) //partly transparent
				{	//apply pixel blending formula
					windowPixel[0] = (r * alpha + windowPixel[0] * (255 - alpha)) / 255; //write image data to the pixel via pointer
					windowPixel[1] = (g * alpha + windowPixel[1] * (255 - alpha)) / 255;
					windowPixel[2] = (b * alpha + windowPixel[2] * (255 - alpha)) / 255;
				}
				//ignore pixels with alpha = 0 (fully transparent)
			}

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

void Graphics::DrawLetter(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, char letter, int posX, int posY)
{
	//based on the resources\ShooterPack\PNG\Interface\Tilemap

	//set to -1 so that only valid characters will get assigned a value
	int tileX = -1; //column on the sheet
	int tileY = -1; //row on the sheet

	//convert lowercase to uppercase as the sheet only has capitals
	if (letter >= 'a' && letter <= 'z')
	{
		letter = letter - 'a' + 'A'; //work out the letter offset and add the capital to convert to uppercase
	}

	if (letter >= 'A' && letter <= 'M') //first letter row in tilesheet
	{
		tileX = letter - 'A'; //A is 0, B is 1...
		tileY = 6;
	}
	else if (letter >= 'N' && letter <= 'Z') //second letter row in tilesheet
	{
		tileX = letter - 'N';
		tileY = 7;
	}
	else if (letter >= '0' && letter <= '9') //digits start 3 tiles in on the symbol row
	{
		tileX = (letter - '0') + 3;
		tileY = 5;
	}
	else if (letter == '%') { tileX = 0; tileY = 5; } //special characters, probably wont use them but still support them
	else if (letter == '+') { tileX = 1; tileY = 5; }
	else if (letter == '-') { tileX = 2; tileY = 5; }

	if (tileX == -1) return; //space or unsupported character, draw nothing

	DrawTilemapImage(window, image, tileSize, tileX, tileY, posX, posY);
}

void Graphics::DrawNumber(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, int value, int posX, int posY)
{
	std::string text = std::to_string(value); //120 becomes 120, -5 becomes -5 but we have special characters supported so nothing bad will happen i hope
	DrawString(window, image, text.c_str(), posX, posY); 
}

void Graphics::DrawString(GamesEngineeringBase::Window& window, GamesEngineeringBase::Image& image, const char* text, int posX, int posY)
{
	for (int i = 0; text[i] != '\0'; i++) //end of a const char* is a '\0'
	{
		DrawLetter(window, image, text[i], posX, posY);
		posX += tileSize; //spaces currently count as nothing so game over will be gameover
	}
}