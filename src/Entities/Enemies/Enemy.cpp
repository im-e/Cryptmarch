#include "Entities/Enemies/Enemy.h"
#include <numbers>


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
	//--------------- ATTACK -----------
	if (isRanged)
	{
		if (fireCooldownTimer > 0) fireCooldownTimer -= deltaTime;
		if (target != nullptr) //only attack if we have a target
		{
			//work out if player is in range
			Vector2f en = getCentrePoint(); //spawner 
			Vector2f pl = target->getCentrePoint(); //player

			float dx = en.x - pl.x; //find the difference of the two points
			float dy = en.y - pl.y; //player to enemy
			float distanceSquared = (dx * dx) + (dy * dy); //If dx² + dy² is less than range², the player is in range
			float rangeSquared = fireRange * fireRange; //range squared

			if (fireCooldownTimer <= 0 && distanceSquared < rangeSquared) //in range and off cooldown
			{
				fireRequested = true;
				fireCooldownTimer = fireCooldown;
			}
		}
	}

	//-------------- MOVE ---------------
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
	{	
		if (target != nullptr) //if target isnt null
		{
			state = EnemyState::Chase; //set to chase
		}
		else
		{
			state = EnemyState::Wander; //otherwise, wander
		}

		switch (state)
		{
			case EnemyState::Wander:
			{
				wanderTimer -= deltaTime; //reduce wander timer by delta time
				if (wanderTimer <= 0) pickWanderDirection(); //if wander timer is <= 0, pick a new direction

				float wanderSpeed = speed * wanderSpeedMultiplier;

				float xMove = wanderX * wanderSpeed * deltaTime;
				float yMove = wanderY * wanderSpeed * deltaTime;

				//pull hitbox origin instead of worldX/Y
				//h for hitbox, had to use this naming convention because of private variables of the same name
				AABB hitbox = getHitbox();

				//test if the enemy is blocked then move
				//i could have done move-then-undo but i didnt want my position to enter an illegal state
				if (!map.isBlockedByWall(hitbox.x + xMove, hitbox.y, hitbox.width, hitbox.height))
				{
					worldX += xMove;
				}
				else
				{
					wanderTimer = 0; //hit a wall, choose a new direction next frame
				}

				hitbox = getHitbox(); //update hitbox after move

				if (!map.isBlockedByWall(hitbox.x, hitbox.y + yMove, hitbox.width, hitbox.height))
				{
					worldY += yMove;
				}
				else
				{
					wanderTimer = 0; //hit a wall, choose a new direction next frame
				}


				break;
			}
			case EnemyState::Chase:
			{
				if (target == nullptr) break; //break incase of nullptr

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
				break;
			}
		}


	}

	//iFrame Timer
	updateTimers(deltaTime);

}

void Enemy::pickWanderDirection()
{
	if (rand() % 100 < idleChance) //if 1-100 is less than idle chance
	{
		wanderX = 0; //set wander to 0 = stand still
		wanderY = 0;
		wanderTimer = 1.0f; //only want to idle for a short time
	}
	else
	{
		//random angle around a full circle (0 -> 2*pi radians = 0 -> 360)
		float angle = Math::randomFloat(0, 2 * std::numbers::pi);
		wanderX = cos(angle); //cos/sin of the same angle
		wanderY = sin(angle); //gives a vector of length 1
		wanderTimer = Math::randomFloat(1.0, 3.0); //random time between 1-3
	}
}

void Enemy::applyKnockback(float dirX, float dirY, float force, float duration)
{
	knockbackVelX = dirX * force; //multiply the knockback by force
	knockbackVelY = dirY * force;
	knockbackTimer = duration; //give it a duration of knockback, should be quite short
}

Projectile Enemy::fireProjectile()
{
	//get player's hitbox to spawn the projectile just outside them
	AABB player = getHitbox();

	Projectile proj;
	proj.x = player.x + (player.width / 2.0f);
	proj.y = player.y + (player.height / 2.0f);

	//subtract the npc position from the player position to give a direction arrow
	Vector2f targetMiddle = target->getCentrePoint();
	Vector2f middle = getCentrePoint();
	float dirX = targetMiddle.x - middle.x;
	float dirY = targetMiddle.y - middle.y;

	//work out the length
	float length = sqrt(dirX * dirX + dirY * dirY);

	//normalise to give us a direction vector
	if (length > 0.0f) //if the length is greater than 0
	{
		dirX /= length; //divide the components by it 
		dirY /= length;
	}

	proj.dirX = dirX;
	proj.dirY = dirY;
	proj.speed = arrowSpeed;
	proj.lifetime = arrowLifetime;
	proj.damage = arrowDamage;
	proj.owner = ProjectileOwner::Enemy;

	proj.knockbackDuration = arrowKnockbackDuration;
	proj.knockbackForce = arrowKnockbackForce;

	proj.image = projectileImage;
	proj.tileX = projectileTileX;
	proj.tileY = projectileTileY;

	return proj;

}

