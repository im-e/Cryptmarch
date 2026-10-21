#include "Camera.h"
#include "Map.h"

void Camera::follow(GamesEngineeringBase::Window& window, Map& map, int followPosX, int followPosY, bool fixedMap)
{
	//center the camera on the target.
	//camX and camY represent the top left corner of the screen view not the center
	//if camX = followPosX, player would be stuck in the top left of the window
	camX = followPosX - (window.getWidth() / 2); //so we find the middle of the scren and offset our camera by that amount to keep the player central
	camY = followPosY - (window.getHeight() / 2); 

	//if its a fixed map, clamp the camera
	if (fixedMap)
	{
		//stop the camera from showing past the left/top edges
		if (camX < 0) camX = 0; //if x/y is offscreen clamp it to 0
		if (camY < 0) camY = 0;

		//stop the camera from showing past the right/bottom edges
		//since cam represents the top left corner of the screen we have to subtract the width/height
		if (camX > map.getMapWidth() - window.getWidth())
		{
			camX = map.getMapWidth() - window.getWidth();
		}

		if (camY > map.getMapHeight() - window.getHeight())
		{
			camY = map.getMapHeight() - window.getHeight();
		}

	}
}