#pragma once
#include "Entity.h"


class Enemy : public Entity
{
private:
	Entity* target;
	int contactDamage;

	float arrowKnockbackForce = 300.f;
	float attackKnockbackDuration = 0.15f;

	float shootCooldown = 0.0f;
	float shootInterval = 2.0f;

	float knockbackVelX = 0.0f;
	float knockbackVelY = 0.0f;
	float knockbackTimer = 0.0f;

public:

	Enemy(EntityStats stats, GamesEngineeringBase::Image* _image, Entity* _target, int _damage) : Entity(stats, _image)
	{
		contactDamage = _damage;
		target = _target;
	}

	int getDamage() const { return contactDamage; }
	float getKnockbackDuration() const { return attackKnockbackDuration; }
	float getKnockbackForce() const { return arrowKnockbackForce; }

	void applyKnockback(float dirX, float dirY, float force, float duration);

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;
	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;

};
