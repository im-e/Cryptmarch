#include "Player.h"
#include "Graphics.h"

bool Player::load(const std::string& path)
{
	return playerImage.load(path);
}

void Player::update(GamesEngineeringBase::Window& window, float deltaTime)
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
	playerX += dirX * playerSpeed * deltaTime;
	playerY += dirY * playerSpeed * deltaTime;

	//clamp the player's position to stay within the window boundaries
	if (playerX < 0.0f)
	{
		playerX = 0.0f;
	}
	else if (playerX > window.getWidth() - playerWidth)
	{
		playerX = window.getWidth() - playerWidth;
	}

	if (playerY < 0.0f)
	{
		playerY = 0.0f;
	}
	else if (playerY > window.getHeight() - playerHeight)
	{
		playerY = window.getHeight() - playerHeight;
	}
}

void Player::draw(GamesEngineeringBase::Window& window)
{
	Graphics::DrawImage(window, playerImage, playerX, playerY);

}