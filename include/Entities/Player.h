#pragma once
#include "GamesEngineeringBase.h"
#include "Entity.h"
#include "Utilities/Vector.h"
#include "Utilities/Collisions.h"
#include "Enemy.h"

enum class AttackState { Ready, Active, Cooldown};

class Player : public Entity
{

private:
	AttackState attackState = AttackState::Ready;
	float attackActiveTimer = 0.0f;
	float cooldownTimer = 0.0f;

	const float attackDuration = 0.15f;
	const float attackCooldown = 0.35f;
	const float attackReach = 32.0f;

	const float attackKnockbackForce = 250.0f;
	const float attackKnockbackDuration = 0.12f;

	float knockbackVelX = 0.0f;
	float knockbackVelY = 0.0f;
	float knockbackTimer = 0.0f;

	int meleeDamage = 35;

	Vector<Enemy*> swingEnemies;

	GamesEngineeringBase::Image* weaponImage;
	int weaponImageXOffset;
	int weaponImageYOffset;

	int keyUp = VK_UP;
	int keyDown = VK_DOWN;
	int keyLeft = VK_LEFT;
	int keyRight = VK_RIGHT;
	int keyAttack = VK_SPACE;

	int facingDirX = 0;
	int facingDirY = 0;

public:
	Player(int _health, float _iFrames, int _speed, float _xPos, float _yPos, GamesEngineeringBase::Image* _image, int _tileXOffset, int _tileYOffset, GamesEngineeringBase::Image* _weaponImage, int _weaponXOffset, int _weaponYOffset) :
		Entity(_health,  _iFrames, _speed, _xPos, _yPos, _image, _tileXOffset, _tileYOffset)
	{
		weaponImage = _weaponImage;
		weaponImageXOffset = _weaponXOffset;
		weaponImageYOffset = _weaponYOffset;
	}

	void applyKnockback(float dirX, float dirY, float force, float duration);

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;
	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;

	AABB getMeleeHitbox() const;
	bool checkMeleeCollision(Enemy* enemy) const;
	void processMelee(Vector<Enemy*>& enemies); 
};