#pragma once
#include "GamesEngineeringBase.h"
#include "Entities/Entity.h"
#include "Utilities/Vector.h"
#include "Utilities/Collisions.h"
#include "Entities/Projectile.h"
#include "Entities/Enemies/Enemy.h"
#include "Entities/Spawner.h"
#include "Entities/Players/CharacterType.h"

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
	Vector<Spawner*>& spawners; //spawners to destory
};

class Player : public Entity
{
protected:
	//mana
	int mana; //current mana
	int maxMana; //maximum mana
	
	int playerId; //id for each player multiplayer
	CharacterType characterType; //what type the player is

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
	float facingDirY = 1; //facing direction for when an attack is made
	//set to 0,1 so that player faces a direction when starting to prevent unintended behaviour

	bool leashActive = false; //leashing for multiplayer
	float leashMinX = 0.0f; //min/max values for leashing
	float leashMaxX = 0.0f;
	float leashMinY = 0.0f;
	float leashMaxY = 0.0f;



public:
	Player(int id, CharacterType charType, EntityStats stats, GamesEngineeringBase::Image* _image, Controls controls) :
		Entity(stats, _image)
	{
		playerId = id;
		characterType = charType;
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

	void setLeash(float minX, float maxX, float minY, float maxY) //set leash for multiplayer
	{
		leashMinX = minX;
		leashMaxX = maxX;
		leashMinY = minY;
		leashMaxY = maxY;
		leashActive = true;
	}

	void clearLeash() { leashActive = false; } //clear leash

	bool leashAllowsMove(float currentPos, float newPos, float leashMin, float leashMax);

	int getMana() { return mana; }
	int getMaxMana() { return maxMana; }

	int getId() { return playerId; }

	void restoreMana(int amount)
	{
		mana += amount;
		if (mana > maxMana) mana = maxMana; //never go above max
	}

};