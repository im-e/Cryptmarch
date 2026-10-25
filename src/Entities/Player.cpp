#include "Entities/Player.h"
#include "Rendering/Graphics.h"

void Player::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	int screenX = cam.worldToScreenX(worldX);
	int screenY = cam.worldToScreenY(worldY);

	Graphics::DrawTilemapImage(window, *image, tileSize, tileXOffset, tileYOffset, screenX, screenY);

	if (attackState == AttackState::Active)
	{
		AABB attackBox = getMeleeHitbox();

		// Convert world coordinates to screen coordinates using the camera
		int screenX = static_cast<int>(attackBox.x - cam.getX());
		int screenY = static_cast<int>(attackBox.y - cam.getY());

		Graphics::DrawTilemapImage(window, *weaponImage, tileSize, weaponImageXOffset, weaponImageYOffset, screenX, screenY);
	}

#ifdef _DEBUG

	AABB hitbox = getHitbox();

	Graphics::DrawRectOutline(window, hitbox.x - cam.getX() , hitbox.y - cam.getY(), hitboxWidth, hitboxHeight, 0, 255, 0);

#endif
}

void Player::update(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
	//MOVEMENT ------------------------------------
	float xMove = 0.0f;
	float yMove = 0.0f;

	if (knockbackTimer > 0.0f) //if player is getting knocked back
	{
		knockbackTimer -= deltaTime;
		xMove = knockbackVelX * deltaTime;
		yMove = knockbackVelY * deltaTime;
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

	//dont need to clamp player positions to the world anymore


	//------------------- ATTACK ----------------------

	//iFrame Timer
	updateTimers(deltaTime);

	if (attackState == AttackState::Ready) //if attack is ready
	{
		if (window.keyPressed(keyAttack)) //if attack pressed
		{
			attackState = AttackState::Active; //set attack state to active
			attackActiveTimer = attackDuration; //set the timer to the duration
			swingEnemies.clear(); // Reset hit history for new swing
		}
	}
	else if (attackState == AttackState::Active)
	{
		attackActiveTimer -= deltaTime; //reduce the attack hitbox duration
		if (attackActiveTimer <= 0.0f) //if its less than/equal to 0
		{
			attackState = AttackState::Cooldown; //attack is on cooldown
			cooldownTimer = attackCooldown; //attack timer is equal to the cooldown
		}
	}
	else if (attackState == AttackState::Cooldown) //if in cooldown
	{
		cooldownTimer -= deltaTime; //reduce the (now) cooldown timer
		if (cooldownTimer <= 0.0f) //if cooldown is over
		{
			attackState = AttackState::Ready; //attack is now ready
		}
	}

}
AABB Player::getMeleeHitbox() const
{
	//get the players normal hitbox
	AABB playerBox = getHitbox();
	//offset it forward using facing direction and reach distance
	float attackX = playerBox.x + (facingDirX * attackReach); 
	float attackY = playerBox.y + (facingDirY * attackReach);

	//return an AABB struct
	return { attackX, attackY, playerBox.width, playerBox.height };
}

bool Player::checkMeleeCollision(Enemy* enemy) const
{
	//get the player's melee hitbox
	AABB attackBox = getMeleeHitbox();

	//get the enemy's hitbox
	AABB enemyBox = enemy->getHitbox();

	//AABB overlap test
	return Collisions::checkAABBCollision(attackBox, enemyBox);
}

void Player::processMelee(Vector<Enemy*>& enemies) {

	if (attackState != AttackState::Active) return;

	for (int i = 0; i < enemies.getSize(); i++) //for every enemy in enemies 
	{
		Enemy* enemy = enemies[i]; 

		//check if this enemy was already hit during this swing
		bool alreadyHit = false;
		for (int k = 0; k < swingEnemies.getSize(); k++)
		{
			if (swingEnemies[k] == enemy) //i dont have a comparison operator this might not work
			{
				alreadyHit = true;
				break;
			}
		}

		if (alreadyHit) continue; //skip if already hit

		//check collision
		if (checkMeleeCollision(enemy))
		{
			enemy->takeDamage(meleeDamage); //make enemy take the damage

			//apply knockback
			enemy->applyKnockback(facingDirX, facingDirY, attackKnockbackForce, attackKnockbackDuration);

			swingEnemies.add(enemy); //record that we hit this enemy
		}
	}
}

void Player::applyKnockback(float dirX, float dirY, float force, float duration)
{
	knockbackVelX = dirX * force;
	knockbackVelY = dirY * force;
	knockbackTimer = duration;
}

