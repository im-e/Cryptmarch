#include "Player.h"
#include "Graphics.h"

bool Player::loadImage(const std::string& path)
{
	return playerImage.load(path);
}

void Player::update(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
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
	float xMove = dirX * playerSpeed * deltaTime;
	float yMove = dirY * playerSpeed * deltaTime;


	//test if the player is blocked then move
	//i could have done move-then-undo but i didnt want my position to enter an illegal state
	if (!map.isBlockedByWall(worldX + xMove, worldY, hitboxWidth, hitboxHeight))
	{
		worldX += xMove;
	}

	if (!map.isBlockedByWall(worldX, worldY + yMove, hitboxWidth, hitboxHeight))
	{
		worldY += yMove;
	}



	//clamp the player's position to the world
	if (worldX < 0.0f)
	{
		worldX = 0.0f;
	}
	else if (worldX > map.getMapWidth() - hitboxWidth)
	{
		worldX = map.getMapWidth() - hitboxWidth;
	}

	if (worldY < 0.0f)
	{
		worldY = 0.0f;
	}
	else if (worldY > map.getMapHeight() - hitboxHeight)
	{
		worldY = map.getMapHeight() - hitboxHeight;
	}
}

void Player::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	int screenX = cam.worldToScreenX(worldX);
	int screenY = cam.worldToScreenY(worldY);

	Graphics::DrawTilemapImage(window, playerImage, playerTileSize, tileXOffset, tileYOffset, screenX, screenY);

}