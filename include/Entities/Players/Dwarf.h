#pragma once
#include "Entities/Player.h"


enum class AttackState { Ready, Active, Cooldown };

class Dwarf : public Player
{
private:
	//attacks
	AttackState attackState = AttackState::Ready; //attack state
	float attackActiveTimer = 0.0f; //timer for how long the attack is active
	float attackCooldownTimer = 0.0f; //cooldown for the attack

	const float attackDuration = 0.15f; //duration of the melee attack
	const float attackCooldown = 0.35f; //cooldown duration of the melee attack
	const float attackReach = 32.0f; //reach of the melee attack

	const float attackKnockbackForce = 250.0f; //force for knockback of the melee attack
	const float attackKnockbackDuration = 0.12f; //duration of knockback for the melee attack
	
	int meleeDamage = 35; //damage of melee attack

	int attackDirX = 0; //direction of melee attack
	int attackDirY = 0; //direction of melee attack

	Vector<Enemy*> swingEnemies; //vector of enemies hit by last swing

	GamesEngineeringBase::Image* weaponImage; //image for weapon
	int weaponImageXOffset; //tilemap offset for weapon
	int weaponImageYOffset;
	
	bool keyWasReleased = true; //check to see if key has been released so multiple activations dont happen on the same frame
	float speedBoostTimer = 0.0f;
	float speedBoostDuration = 3.0f;
	float normalSpeed = 200.0f;
	float boostedSpeed = 330.0f;

	int maxMana = 100;
	int manaCost = 25;
public:

	Dwarf(EntityStats _stats, GamesEngineeringBase::Image* _image, Controls _controls, GamesEngineeringBase::Image* _weaponImage, int _weaponXOffset, int _weaponYOffset) :
		Player(_stats, _image, _controls)
	{
		weaponImage = _weaponImage;
		weaponImageXOffset = _weaponXOffset;
		weaponImageYOffset = _weaponYOffset;

		speed = normalSpeed;
		maxHealth = 150;
		maxMana = 100;
		health = maxHealth;
		mana = maxMana;
	}

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;
	void processAttack(Vector<Enemy*>& enemies) override;

	AABB getMeleeHitbox() const;
	bool checkMeleeCollision(Enemy* enemy) const;

};