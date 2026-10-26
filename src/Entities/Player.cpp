#include "Entities/Player.h"
#include "Rendering/Graphics.h"

void Player::applyImpulse(float dirX, float dirY, float force, float duration)
{
	impulseVelX = dirX * force;
	impulseVelY = dirY * force;
	impulseTimer = duration;
}
void Player::move(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
	//MOVEMENT ------------------------------------
	float xMove = 0.0f;
	float yMove = 0.0f;

	if (impulseTimer > 0.0f) //if player is getting knocked back
	{
		impulseTimer -= deltaTime;
		xMove = impulseVelX * deltaTime;
		yMove = impulseVelY * deltaTime;
	}
	else { //player moves normally
		//get the direction of movement based on key presses
		float dirX = 0.0f;
		float dirY = 0.0f;

		//check for key presses and update direction accordingly
		if (window.keyPressed(keyLeft)) { dirX -= 1.0f; }
		if (window.keyPressed(keyRight)) { dirX += 1.0f; }
		if (window.keyPressed(keyUp)) { dirY -= 1.0f; }
		if (window.keyPressed(keyDown)) { dirY += 1.0f; }

		//check if moving diagonally and normalize the direction vector
		if (dirX != 0.0f && dirY != 0.0f)
		{
			float length = sqrt(dirX * dirX + dirY * dirY);
			dirX /= length;
			dirY /= length;
		}

		//only update facing direction if there is movement input
		if (dirX != 0.0f || dirY != 0.0f)
		{
			//if were moving diagonally, normalize facing direction
			float len = sqrt(dirX * dirX + dirY * dirY);
			facingDirX = dirX / len;
			facingDirY = dirY / len;
		}

		//apply the movement to the player's position based on speed and deltaTime
		//use x/yMove to track how far the player wants to move
		xMove = dirX * speed * deltaTime;
		yMove = dirY * speed * deltaTime;
	}

	//----------- COLLISION AND POSITION UPDATE
	//pull hitbox origin instead of worldX/Y
	//h for hitbox, had to use this naming convention because of private variables of the same name
	AABB hitbox = getHitbox();

	//test if the player is blocked then move
	//i could have done move-then-undo but i didnt want my position to enter an illegal state
	if (!map.isBlockedByWall(hitbox.x + xMove, hitbox.y, hitbox.width, hitbox.height))
	{
		worldX += xMove;
	}

	hitbox = getHitbox(); //update hitbox after move

	if (!map.isBlockedByWall(hitbox.x, hitbox.y + yMove, hitbox.width, hitbox.height))
	{
		worldY += yMove;
	}

	updateTimers(deltaTime);
}