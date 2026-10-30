#pragma once
#include "Entities/Players/Player.h"



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

	const float knockbackForce = 300.0f; //force for knockback of the melee attack
	const float knockbackDuration = 0.18f; //duration of knockback for the melee attack
	
	int meleeDamage = 75; //damage of melee attack

	int attackDirX = 0; //direction of melee attack
	int attackDirY = 0; //direction of melee attack

	Vector<Enemy*> swingEnemies; //vector of enemies hit by last swing
	Vector<Spawner*> swingSpawners; //vector of spawners hit by last swing

	GamesEngineeringBase::Image* weaponImage; //image for weapon
	int weaponImageXOffset; //tilemap offset for weapon
	int weaponImageYOffset;
	
	bool keyWasReleased = true; //check to see if key has been released so multiple activations dont happen on the same frame
	float skillTimer = 0.0f; //timer for speed boost
	float skillDuration = 3.0f; //speed boost duration
	const float defenceMultiplier = 0.5f;
	float normalSpeed = 300.0f; //nomral movement speed
	float boostedSpeed = 440.0f; //boosted movement speed

	int manaCost; //mana cost for secondary skill
public:

	Dwarf(EntityStats _stats, GamesEngineeringBase::Image* _image, Controls _controls, GamesEngineeringBase::Image* _weaponImage, int _weaponXOffset, int _weaponYOffset) :
		Player(_stats, _image, _controls)
	{
		weaponImage = _weaponImage;
		weaponImageXOffset = _weaponXOffset;
		weaponImageYOffset = _weaponYOffset;

		maxHealth = 150; //much tankier than other heros
		maxMana = 70; //less mana than others
		speed = normalSpeed; //normal speed is 300 which is much faster
		manaCost = 25; 
		health = maxHealth;
		mana = maxMana;
	}

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;
	void processAttack(AttackContext& attack) override;

	AABB getMeleeHitbox() const;
	bool checkMeleeCollision(Entity* entity) const;

};