#pragma once
#include "Entities/Entity.h"
#include "Entities/Projectile.h"

enum class EnemyState { Wander, Chase };

class Enemy : public Entity
{
private:
	//image used for attack (arrows)
	GamesEngineeringBase::Image* projectileImage;
	int projectileTileX;
	int projectileTileY;

	//ranged attack 
	float arrowSpeed = 300.0f; //speed that the arrow travels
	float arrowLifetime = 3.0f; //lifetime of the arrow
	int arrowDamage = 20; //damage of the arrow
	float fireCooldown = 1.0f; //cooldown to fire again
	float fireCooldownTimer = 0.0f; //cooldown tracker
	bool fireRequested = false; //if fire requested
	float fireRange = 2000; //range of fire
	float arrowKnockbackForce = 150.0f; //force for knockback of the ranged attack
	float arrowKnockbackDuration = 0.10f; //duration of knockback for the ranged attack

	Entity* target = nullptr; //target enemy will path towards
	float aggroRange = 350.f; //start chasing when player comes inside this
	float leashRange = 550.f; //stop chasing when player has left this range

	EnemyState state = EnemyState::Wander; //movement state of enemies
	float wanderX = 0.0f; //x and y of wander movement
	float wanderY = 0.0f;
	float wanderTimer = 0.0f; //timer for wandering
	float wanderSpeedMultiplier = 0.5f; //enemies will move slower while in wander state
	int idleChance = 25; //chance to idle instead of moving

	int contactDamage; //damage enemy deals on contact

	float knockbackForce; //force of knockback enemies will deal
	float knockbackDuration = 0.3f; //duration of knockback enemies will deal

	float shootCooldown = 0.0f; //cooldown timer for ranged enemies
	float shootInterval = 2.0f; //interval at which enemies can shoot at

	float knockbackVelX = 0.0f; //knockback velocities for enemy when it gets hit
	float knockbackVelY = 0.0f;
	float knockbackTimer = 0.0f;

	bool isRanged; //if enemy is ranged

public:

	Enemy(EntityStats _stats, GamesEngineeringBase::Image* _image, int _damage, int _knockbackForce, bool _isRanged,
	GamesEngineeringBase::Image* _projectileImage, int _projectileOffsetX, int _projectileOffsetY) : Entity(_stats, _image)
	{
		contactDamage = _damage;
		knockbackForce = _knockbackForce;
		isRanged = _isRanged;
		projectileImage = _projectileImage;
		projectileTileX = _projectileOffsetX;
		projectileTileY = _projectileOffsetY;
	}

	void setTarget(Entity* player) { target = player; }
	void setFireRequested(bool _fireRequested) { fireRequested = _fireRequested; }

	Entity* getTarget() { return target; }
	bool getFireRequested() const { return fireRequested; }
	int getDamage() const { return contactDamage; }
	float getKnockbackDuration() const { return knockbackDuration; }
	float getKnockbackForce() const { return knockbackForce; }
	float getAggroRange() const { return aggroRange; }
	float getLeashRange() const { return leashRange; }

	void applyKnockback(float dirX, float dirY, float force, float duration);
	void pickWanderDirection();

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;
	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;

	Projectile fireProjectile();
};
