#pragma once
#include "GamesEngineeringBase.h"
#include "Camera.h"
#include "Map.h"
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

	//image tile settings
	int tileSize = 32; //tilesize
	int tileXOffset;
	int tileYOffset;

	int hitboxWidth = 32;
	int hitboxHeight = 32;



	//image
	GamesEngineeringBase::Image image;
	bool loadImage(const std::string& path);


public:
	Entity(int _health, float _speed, float _x, float _y, const std::string& path, int _tileXOffset, int _tileYOffset)
	{
		if (!loadImage(path)) { //if load returns false
			Logger::Error("Player Tilemap failed to load at filepath: " + path); //assert
		}
		health = _health;
		speed = _speed;
		worldX = _x;
		worldY = _y;
		tileXOffset = _tileXOffset;
		tileYOffset = _tileYOffset;
	}

	virtual void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) = 0;
	void draw(GamesEngineeringBase::Window& window, Camera& cam);


	float getX() { return worldX; }
	float getY() { return worldY; }

	int getHealth() { return health; }

};