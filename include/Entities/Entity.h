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

	void updateTimers(float deltaTime);

	float getX() { return worldX; } 
	float getY() { return worldY; }

	int getHealth() { return health; }
	bool isDead() const;
	void takeDamage(int damage);
	void heal(int amount) { health += amount; }

	void setHitboxSize(int width, int height);
	AABB getHitbox() const;

	bool checkEntityCollision(const Entity* collider) const;
};