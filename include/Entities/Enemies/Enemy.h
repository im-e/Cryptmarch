#pragma once
#include "Entities/Entity.h"
#include "Entities/Projectile.h"


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



	Entity* target;
	int contactDamage;

	float knockbackForce;
	float knockbackDuration = 0.15f;

	float shootCooldown = 0.0f;
	float shootInterval = 2.0f;

	float knockbackVelX = 0.0f;
	float knockbackVelY = 0.0f;
	float knockbackTimer = 0.0f;

	bool isRanged;

public:

	Enemy(EntityStats _stats, GamesEngineeringBase::Image* _image, Entity* _target, int _damage, int _knockbackForce, bool _isRanged,
	GamesEngineeringBase::Image* _projectileImage, int _projectileOffsetX, int _projectileOffsetY) : Entity(_stats, _image)
	{
		contactDamage = _damage;
		target = _target;
		knockbackForce = _knockbackForce;
		isRanged = _isRanged;
		projectileImage = _projectileImage;
		projectileTileX = _projectileOffsetX;
		projectileTileY = _projectileOffsetY;
	}

	bool getFireRequested() const { return fireRequested; }
	void setFireRequested(bool _fireRequested) { fireRequested = _fireRequested; }

	int getDamage() const { return contactDamage; }
	float getKnockbackDuration() const { return knockbackDuration; }
	float getKnockbackForce() const { return knockbackForce; }

	void applyKnockback(float dirX, float dirY, float force, float duration);

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;
	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;

	Projectile fireProjectile();
};
