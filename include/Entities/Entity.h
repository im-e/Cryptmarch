#pragma once
#include "GamesEngineeringBase.h"
#include "Rendering/Camera.h"
#include "Map/Map.h"
#include "Utilities/Logger.h"


class Entity {
protected:
	//entity data
	int health; //health

	//position
	float worldX;
	float worldY;

	//movement
	float speed; // pixels per second
	
	//state
	bool isAlive = true;

	//invincibility frames
	float iFrames = .5f;

	//image tile settings
	int tileSize = 32; //tilesize
	int tileXOffset;
	int tileYOffset;

	//hitbox settings
	int hitboxWidth = 20;
	int hitboxHeight = 20;

	//image
	GamesEngineeringBase::Image* image;


public:
	Entity(int _health, float _speed, float _x, float _y, GamesEngineeringBase::Image* _image, int _tileXOffset, int _tileYOffset)
	{
		image = _image;
		health = _health;
		speed = _speed;
		worldX = _x;
		worldY = _y;
		tileXOffset = _tileXOffset;
		tileYOffset = _tileYOffset;
	}

	virtual ~Entity() = default;

	virtual void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) = 0;
	void draw(GamesEngineeringBase::Window& window, Camera& cam);

	float getX() { return worldX; }
	float getY() { return worldY; }

	int getHealth() { return health; }

	bool isDead() const;

	void takeDamage(int damage);
	void heal(int amount) { health += amount; }

	void setHitboxSize(int width, int height);

	void getHitbox(float& x, float& y, int& width, int& height) const;

	bool checkCollision(const Entity* collider) const;
};