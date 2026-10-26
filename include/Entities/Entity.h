#pragma once
#include "GamesEngineeringBase.h"
#include "Rendering/Camera.h"
#include "Map/Map.h"
#include "Utilities/Logger.h"
#include "Utilities/Collisions.h"

struct EntityStats {
	int health;
	int maxHealth;
	float iFrames;
	float speed;
	float spawnX;
	float spawnY;
	int imageXOffset;
	int imageYOffset;
};


class Entity {
protected:
	//entity data
	int health; //health
	int maxHealth;
	float damageTakenMultiplier = 1.0f; //damage multiplier

	float hitFlashTimer = 0.0f;
	const float hitFlashDuration = 0.1f;

	//position
	float worldX;
	float worldY;

	//movement
	float speed; // pixels per second
	
	//state
	bool isAlive = true;

	//invincibility frames
	float iFrameTimer;
	float iFrameValue;

	//image tile settings
	int tileSize = 32; //tilesize
	int tileXOffset;
	int tileYOffset;

	//hitbox settings
	int hitboxWidth = 20
		;
	int hitboxHeight = 20;

	//image
	GamesEngineeringBase::Image* image;


public:
	Entity(EntityStats stats, GamesEngineeringBase::Image* _image)
	{
		health = stats.maxHealth;
		maxHealth = stats.maxHealth;
		iFrameValue = stats.iFrames;
		iFrameTimer = stats.iFrames;
		speed = stats.speed;
		worldX = stats.spawnX;
		worldY = stats.spawnY;
		image = _image;
		tileXOffset = stats.imageXOffset;
		tileYOffset = stats.imageYOffset;
	}

	virtual ~Entity() = default;

	virtual void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) = 0;
	virtual void draw(GamesEngineeringBase::Window& window, Camera& cam) = 0;

	void drawSprite(GamesEngineeringBase::Window& window, Camera& cam, unsigned char tintR = 0, unsigned char tintG = 0, unsigned char tintB = 0, unsigned char tintStrength = 0);

	float getX() const { return worldX; }
	float getY() const { return worldY; }

	int getHealth() const { return health; }
	int getMaxHealth() const { return maxHealth; }

	bool isFlashing() const { return hitFlashTimer > 0.0f; }

	bool isDead() const;
	void takeDamage(int damage);
	void heal(int amount) { health += amount; }

	void updateTimers(float deltaTime);

	void setHitboxSize(int width, int height);
	AABB getHitbox() const;

	bool checkEntityCollision(const Entity* collider) const;
};