#pragma once
#include "Entities/Entity.h"


class Enemy : public Entity
{
private:
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

	Enemy(EntityStats _stats, GamesEngineeringBase::Image* _image, Entity* _target, int _damage, int _knockbackForce, bool _isRanged) : Entity(_stats, _image)
	{
		contactDamage = _damage;
		target = _target;
		knockbackForce = _knockbackForce;
		isRanged = _isRanged;
	}

	int getDamage() const { return contactDamage; }
	float getKnockbackDuration() const { return knockbackDuration; }
	float getKnockbackForce() const { return knockbackForce; }

	void applyKnockback(float dirX, float dirY, float force, float duration);

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;
	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;

};
