#include "Entities/Player.h"
#include "Rendering/Graphics.h"


void Player::update(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
	//MOVEMENT ------------------
	//get the direction of movement based on key presses
	float dirX = 0.0f;
	float dirY = 0.0f;

	//check for key presses and update direction accordingly
	if (window.keyPressed(keyLeft))
	{
		dirX -= 1.0f;
	}
	if (window.keyPressed(keyRight))
	{
		dirX += 1.0f;
	}
	if (window.keyPressed(keyUp))
	{
		dirY -= 1.0f;
	}
	if (window.keyPressed(keyDown))
	{
		dirY += 1.0f;
	}

	//check if moving diagonally and normalize the direction vector
	if (dirX != 0.0f && dirY != 0.0f)
	{
		float length = sqrt(dirX * dirX + dirY * dirY);
		dirX /= length;
		dirY /= length;
	}

	//check if the player is trying to move through an impassable wall
	//if so stop the movement

	//apply the movement to the player's position based on speed and deltaTime
	//use x/yMove to track how far the player wants to move
	float xMove = dirX * speed * deltaTime;
	float yMove = dirY * speed * deltaTime;


	//pull hitbox origin instead of worldX/Y
	//h for hitbox, had to use this naming convention because of private variables of the same name
	float hX, hY;
	int hWidth, hHeight;
	getHitbox(hX, hY, hWidth, hHeight);


	//test if the player is blocked then move
	//i could have done move-then-undo but i didnt want my position to enter an illegal state
	if (!map.isBlockedByWall(hX + xMove, hY, hWidth, hHeight))
	{
		worldX += xMove;
	}

	getHitbox(hX, hY, hWidth, hHeight); //update hitbox after move

	if (!map.isBlockedByWall(hX, hY + yMove, hWidth, hHeight))
	{
		worldY += yMove;
	}

	getHitbox(hX, hY, hWidth, hHeight); //update hitbox after move again

	//clamp the player's position to the world using hitbox
	if (hX < 0.0f) //left
	{
		worldX = 0.0f;
	}
	else if (hX > map.getMapWidth() - hWidth) //right
	{
		worldX = map.getMapWidth() - hWidth;
	}
	if (hY < 0.0f) //top
	{
		worldY = 0.0f;
	}
	else if (hY > map.getMapHeight() - hHeight) //bottom
	{
		worldY = map.getMapHeight() - hHeight;
	}

	//iFrame Timer
	updateTimers(deltaTime);
}
