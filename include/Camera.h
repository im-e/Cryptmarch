#pragma once
#include "GamesEngineeringBase.h"

class Map;

class Camera
{
private:
	int camX = 0;
	int camY = 0;
public:

	int getX() const { return camX; } //getters
	int getY() const { return camY; }

	void setX(int x_) { camX = x_; } //setters
	void setY(int y_) { camY = y_; }
	void setPos(int x_, int y_) { camX = x_; camY = y_; }

	int worldToScreenX(int worldX) const //convert world x to screen x
	{
		//by subtracting the cameras position from the world position we find out where the object should be drawn on the window
		return worldX - camX; 
	}

	int worldToScreenY(int worldY) const //convert world y to screen y
	{
		return worldY - camY;
	}

	void follow(GamesEngineeringBase::Window& window, Map& map, int followPosX, int followPosY, bool fixedMap);

};