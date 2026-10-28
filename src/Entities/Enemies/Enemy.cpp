#include "Entities/Enemies/Enemy.h"


void Enemy::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	drawSprite(window, cam);

#ifdef _DEBUG

	AABB hitbox = getHitbox();

	Graphics::DrawRectOutline(window, cam.worldToScreenX(hitbox.x), cam.worldToScreenY(hitbox.y), hitboxWidth, hitboxHeight, 255, 0, 0);

#endif

}

void Enemy::update(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
	//if in knockback
	if (knockbackTimer > 0.0f)
	{	//knock enemy back
		knockbackTimer -= deltaTime;

		// Pull hitbox for wall checks

		AABB hitbox = getHitbox();

		float xMove = knockbackVelX * deltaTime;
		float yMove = knockbackVelY * deltaTime;

		//move with wall collision checks so enemies don't fly through walls
		if (!map.isBlockedByWall(hitbox.x + xMove, hitbox.y, hitboxWidth, hitboxHeight))
		{
			worldX += xMove;
		}

		hitbox = getHitbox(); //update hitbox after move

		if (!map.isBlockedByWall(hitbox.x, hitbox.y + yMove, hitboxWidth, hitboxHeight))
		{
			worldY += yMove;
		}

	}
	else //enemies move normally
	{	//could make it so enemies have a random movement, until they get a target and then move to target
		if (target == nullptr) return;
		//subtract the npc position from the player position to give a direction arrow
		float dirX = target->getX() - worldX;
		float dirY = target->getY() - worldY;


		//each enemy computes sqrt every frame (optimisation potential?)
		//work out the length
		float length = sqrt(dirX * dirX + dirY * dirY);

		//normalise to give us a direction vector
		if (length > 0.0f) //if the length is greater than 0
		{
			dirX /= length; //divide the components by it 
			dirY /= length;
		}

		//apply the movement to the enemy's position based on speed and deltaTime
		//use x/yMove to track how far the enemy wants to move
		float xMove = dirX * speed * deltaTime;
		float yMove = dirY * speed * deltaTime;

		//pull hitbox origin instead of worldX/Y
		//h for hitbox, had to use this naming convention because of private variables of the same name
		AABB hitbox = getHitbox();


		//test if the enemy is blocked then move
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

	}

	//iFrame Timer
	updateTimers(deltaTime);

}

void Enemy::applyKnockback(float dirX, float dirY, float force, float duration)
{
	knockbackVelX = dirX * force; //multiply the knockback by force
	knockbackVelY = dirY * force;
	knockbackTimer = duration; //give it a duration of knockback, should be quite short
}

