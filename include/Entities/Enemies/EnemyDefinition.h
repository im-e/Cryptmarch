#pragma once
#include "Utilities/Vector2f.h"
#include <string>

class EnemyDefinition
{
public:
	int maxHealth;
	float speed;
	int contactDamage;
	Vector2f spriteOffset;
	int knockbackForce;
	bool canShoot;

	EnemyDefinition(int _maxHealth, float _speed, int _contactDamage, Vector2f _spriteOffset, int _knockbackForce, bool _canShoot)
	{
		maxHealth = _maxHealth;
		speed = _speed;
		contactDamage = _contactDamage;
		spriteOffset = _spriteOffset;
		knockbackForce = _knockbackForce;
		canShoot = _canShoot;
	}

};