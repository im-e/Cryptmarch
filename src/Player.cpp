#include "Player.h"
#include "Graphics.h"

bool Player::load(const std::string& path)
{
	return playerImage.load(path);
}

void Player::update(GamesEngineeringBase::Window& window, float deltaTime, float worldWidth, float worldHeight)
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

	//apply the movement to the player's position based on speed and deltaTime
	worldX += dirX * playerSpeed * deltaTime;
	worldY += dirY * playerSpeed * deltaTime;

	//clamp the player's position to the world
	if (worldX < 0.0f)
	{
		worldX = 0.0f;
	}
	else if (worldX > worldWidth - playerWidth)
	{
		worldX = worldWidth - playerWidth;
	}

	if (worldY < 0.0f)
	{
		worldY = 0.0f;
	}
	else if (worldY > worldHeight - playerHeight)
	{
		worldY = worldHeight - playerHeight;
	}
}

void Player::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	int screenX = cam.worldToScreenX(worldX);
	int screenY = cam.worldToScreenY(worldY);

	Graphics::DrawTilemapImage(window, playerImage, playerTileSize, tileXOffset, tileYOffset, screenX, screenY);

}