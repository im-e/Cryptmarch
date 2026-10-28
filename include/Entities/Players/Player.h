#pragma once
#include "GamesEngineeringBase.h"
#include "Entities/Entity.h"
#include "Utilities/Vector.h"
#include "Utilities/Collisions.h"
#include "Entities/Projectile.h"
#include "Entities/Enemies/Enemy.h"

struct Controls
{
	int up;
	int down;
	int left;
	int right;
	int primary;
	int secondary;
};

struct AttackContext{
	Vector<Enemy*>& enemies; //who you can hit
	Vector<Projectile>& projectiles; //where new projectiles go
};



class Player : public Entity
{
protected:
	//mana
	int mana; //current mana
	int maxMana; //maximum mana

	//knockback
	float impulseVelX = 0.0f; //player knockback velocity x 
	float impulseVelY = 0.0f; //player knockback velocity y
	float impulseTimer = 0.0f; //duration of knockback in time

	//movement
	int keyUp; //player movement keys
	int keyDown;
	int keyLeft;
	int keyRight;
	int keyPrimaryAttack; //player attack key
	int keySecondaryAttack; //player attack key
	float facingDirX = 0; //facing direction for when an attack is made
	float facingDirY = 0; //facing direction for when an attack is made

public:
	Player(EntityStats stats, GamesEngineeringBase::Image* _image, Controls controls) :
		Entity(stats, _image)
	{
		mana = 100;
		maxMana = 100;
		keyUp = controls.up;
		keyDown = controls.down;
		keyLeft = controls.left;
		keyRight = controls.right;
		keyPrimaryAttack = controls.primary;
		keySecondaryAttack = controls.secondary;
	}
	virtual ~Player() = default;
	void applyImpulse(float dirX, float dirY, float force, float duration);
	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override = 0;

	virtual void processAttack(AttackContext& attackContext) = 0; 
	void draw(GamesEngineeringBase::Window& window, Camera& cam) override = 0;

	void move(GamesEngineeringBase::Window& window, Map& map, float deltaTime);

	int getMana() { return mana; }
	int getMaxMana() { return maxMana; }

};