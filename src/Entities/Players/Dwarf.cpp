#include "Entities/Players/Dwarf.h"

void Dwarf::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	if (skillTimer > 0.0f) //if skill active
	{
		drawSprite(window, cam, 80, 140, 255, 100); //draw with a blue tint 40% strength while defending
	}
	else //if skill not active
	{
		drawSprite(window, cam); //draw normally
	}

	if (attackState == AttackState::Active) //if the attack is active
	{
		AABB attackBox = getMeleeHitbox(); //get the hitbox

		//convert world coordinates to screen coordinates using the camera
		int screenX = cam.worldToScreenX(attackBox.x);
		int screenY = cam.worldToScreenY(attackBox.y);
		//draw weapon attack to screen
		Graphics::DrawTilemapImage(window, *weaponImage, tileSize, weaponImageXOffset, weaponImageYOffset, screenX, screenY);
	}

#ifdef _DEBUG

	AABB hitbox = getHitbox();

	Graphics::DrawRectOutline(window, cam.worldToScreenX(hitbox.x), cam.worldToScreenY(hitbox.y), hitboxWidth, hitboxHeight, 0, 255, 0);

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
			swingSpawners.clear(); // Reset spawner hit history for new swing

			attackDirX = facingDirX; //lock the attack direction to current facing direction
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
	if (skillTimer > 0.0f) //if speedboost is active
	{
		skillTimer -= deltaTime; //reduce time
		if (skillTimer <= 0.0f) //if time has run out
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

		
			speed = boostedSpeed; //activate faster movement skill
			damageTakenMultiplier = defenceMultiplier; //activate defence skill
			skillTimer = skillDuration; //set the timer to the duration
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
void Dwarf::processAttack(AttackContext& attackContext) {

	if (attackState != AttackState::Active) return;

	for (int i = 0; i < attackContext.enemies.getSize(); i++) //for every enemy in enemies 
	{
		Enemy* enemy = attackContext.enemies[i];

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
			enemy->applyKnockback(attackDirX, attackDirY, arrowKnockbackForce, attackKnockbackDuration);

			swingEnemies.add(enemy); //record that we hit this enemy
		}
	}

	//spawnmers - same check as enemies, but no knockback 
	for (int i = 0; i < attackContext.spawners.getSize(); i++) //for every spawner
	{
		Spawner* spawner = attackContext.spawners[i];

		if (spawner->isDead()) continue; //skip spawners already destroyed this frame

		//check if this spawner was already hit during this swing
		bool alreadyHit = false;
		for (int k = 0; k < swingSpawners.getSize(); k++)
		{
			if (swingSpawners[k] == spawner) 
			{
				alreadyHit = true;
				break;
			}
		}

		if (alreadyHit) continue; //skip if already hit

		if (checkMeleeCollision(spawner)) //check melee collision
		{
			spawner->takeDamage(meleeDamage); //spawner takes the damage
			swingSpawners.add(spawner); //record that we hit this spawner
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

bool Dwarf::checkMeleeCollision(Entity* entity) const
{
	//get the player's melee hitbox
	AABB attackBox = getMeleeHitbox();

	//get the enemy's hitbox
	AABB enemyBox = entity->getHitbox();

	//AABB overlap test
	return Collisions::checkAABBCollision(attackBox, enemyBox);
}