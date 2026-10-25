#pragma once
#include "Entity.h"


class Enemy : public Entity
{
private:
	Entity* target;
	int contactDamage;

	float attackKnockbackForce = 300.f;
	float attackKnockbackDuration = 0.15f;

	float knockbackVelX = 0.0f;
	float knockbackVelY = 0.0f;
	float knockbackTimer = 0.0f;

public:

	Enemy(int _health, int _damage, float _iFrames, float _speed, int _x, int _y, GamesEngineeringBase::Image* _image, int _tileXOffset, int _tileYOffset, Entity* _target) :
		Entity(_health, _iFrames, _speed, _x, _y, _image, _tileXOffset, _tileYOffset)
	{
		contactDamage = _damage;
		target = _target;
	}

	int getDamage() const { return contactDamage; }
	float getKnockbackDuration() const { return attackKnockbackDuration; }
	float getKnockbackForce() const { return attackKnockbackForce; }

	void applyKnockback(float dirX, float dirY, float force, float duration);

	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;
	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;

};
