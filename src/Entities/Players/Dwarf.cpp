#include "Entities/Players/Dwarf.h"

void Dwarf::draw(GamesEngineeringBase::Window& window, Camera& cam)
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

	Graphics::DrawRectOutline(window, hitbox.x - cam.getX(), hitbox.y - cam.getY(), hitboxWidth, hitboxHeight, 0, 255, 0);

#endif
}

void Dwarf::update(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
	//------------------- ATTACK ----------------------
	//PRIMARY ATTACK
	if (attackState == AttackState::Ready) //if attack is ready
	{
		if (window.keyPressed(keyPrimaryAttack)) //if attack pressed
		{
			attackState = AttackState::Active; //set attack state to active
			attackActiveTimer = attackDuration; //set the timer to the duration
			swingEnemies.clear(); // Reset hit history for new swing

			attackDirX = facingDirX;
			attackDirY = facingDirY;
		}
	}
	else if (attackState == AttackState::Active)
	{
		attackActiveTimer -= deltaTime; //reduce the attack hitbox duration
		if (attackActiveTimer <= 0.0f) //if its less than/equal to 0
		{
			attackState = AttackState::Cooldown; //attack is on cooldown
			attackCooldownTimer = attackCooldown; //attack timer is equal to the cooldown
		}
	}
	else if (attackState == AttackState::Cooldown) //if in cooldown
	{
		attackCooldownTimer -= deltaTime; //reduce the (now) cooldown timer
		if (attackCooldownTimer <= 0.0f) //if cooldown is over
		{
			attackState = AttackState::Ready; //attack is now ready
		}
	}

	//SECONDARY ATTACK -- SPEED BOOST AND HEALTH BOOST
	if (speedBoostTimer > 0.0f) //if speedboost is active
	{
		speedBoostTimer -= deltaTime; //reduce time
		if (speedBoostTimer <= 0.0f) //if time has run out
		{
			speed = normalSpeed; //set to normal speed
		}
	}

	bool secondaryPressed = window.keyPressed(keySecondaryAttack); //check if secondary was pressed
	if (secondaryPressed && keyWasReleased) //if it was pressed and key was released
	{
		if (mana >= manaCost) //check if we have the mana to cast
		{
			mana -= manaCost; //reduce mana by the cost

			//activate faster movement skill
			speed = boostedSpeed;
			speedBoostTimer = speedBoostDuration; //set the timer to the duration

			//stronger defence / heal
			health += 30.0f; //heal 
			if (health > maxHealth) health = maxHealth; //cap health
			keyWasReleased = false; //set keywasreleased to false
		}
	}
	else if (!secondaryPressed) //if secondary is not pressed
	{
		keyWasReleased = true; //set key was released to true
	}

	//---------------------- MOVE ----------------
	move(window, map, deltaTime);

}
void Dwarf::processAttack(Vector<Enemy*>& enemies) {

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
			enemy->applyKnockback(attackDirX, attackDirY, attackKnockbackForce, attackKnockbackDuration);

			swingEnemies.add(enemy); //record that we hit this enemy
		}
	}
}

AABB Dwarf::getMeleeHitbox() const
{
	//get the players normal hitbox
	AABB playerBox = getHitbox();
	//offset it forward using facing direction and reach distance
	float attackX = playerBox.x + (attackDirX * attackReach);
	float attackY = playerBox.y + (attackDirY * attackReach);

	//return an AABB struct
	return { attackX, attackY, playerBox.width, playerBox.height };
}

bool Dwarf::checkMeleeCollision(Enemy* enemy) const
{
	//get the player's melee hitbox
	AABB attackBox = getMeleeHitbox();

	//get the enemy's hitbox
	AABB enemyBox = enemy->getHitbox();

	//AABB overlap test
	return Collisions::checkAABBCollision(attackBox, enemyBox);
}