#pragma once
#include "GamesEngineeringBase.h"
#include "Entity.h"
#include "Utilities/Vector.h"
#include "Utilities/Collisions.h"
#include "Enemy.h"

struct Controls
{
	int up;
	int down;
	int left;
	int right;
	int primary;
	int secondary;
};

class Player : public Entity
{

protected:
	int mana;
	int maxMana;

	//knockback
	float knockbackVelX = 0.0f; //player knockback velocity x 
	float knockbackVelY = 0.0f; //player knockback velocity y
	float knockbackTimer = 0.0f; //duration of knockback in time

	//movement
	int keyUp; //player movement keys
	int keyDown;
	int keyLeft;
	int keyRight;
	int keyPrimaryAttack; //player attack key
	int keySecondaryAttack; //player attack key
	int facingDirX = 0; //facing direction for when an attack is made
	int facingDirY = 0; //facing direction for when an attack is made

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
	void applyKnockback(float dirX, float dirY, float force, float duration);
	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override = 0;

	virtual void processAttack(Vector<Enemy*>& enemies) = 0; 
	void draw(GamesEngineeringBase::Window& window, Camera& cam) override = 0;

	void move(GamesEngineeringBase::Window& window, Map& map, float deltaTime);

};