#pragma once
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
		return worldX - camX;
	}

	int worldToScreenY(int worldY) const //convert world y to screen y
	{
		return worldY - camY;
	}


	void follow(int worldX, int worldY, int windowWidth, int windowHeight, bool fixedMap, int mapWidth, int mapHeight)
	{
		//centre the camera on the target.
		camX = worldX - (windowWidth / 2);
		camY = worldY - (windowHeight / 2);

		//if its a fixed map, clamp the camera
		if (fixedMap)
		{
			//stop the camera from showing past the left/top edges
			if (camX < 0) camX = 0;
			if (camY < 0) camY = 0;

			//stop the camera from showing past the right/bottom edges
			if (camX > mapWidth - windowWidth)
				camX = mapWidth - windowWidth;
			if (camY > mapHeight - windowHeight)
				camY = mapHeight - windowHeight;
		}
	}

};