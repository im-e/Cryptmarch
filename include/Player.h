#pragma once
#include "GamesEngineeringBase.h"
#include "Camera.h"
#include "Map.h"

class Player
{

private:
	//image
	GamesEngineeringBase::Image playerImage;
	int playerTileSize = 32; //tilesize
	int tileXOffset = 0;
	int tileYOffset = 0;

	//position
	float worldX = 50.0f;
	float worldY = 100.0f;

	//movement
	float playerSpeed = 200.0f; // pixels per second

	//bounding box size
	int hitboxWidth = 32;
	int hitboxHeight = 32;

	int keyUp = VK_UP;
	int keyDown = VK_DOWN;
	int keyLeft = VK_LEFT;
	int keyRight = VK_RIGHT;

public:
	bool loadImage(const std::string& path);
	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime);
	void draw(GamesEngineeringBase::Window& window, Camera& cam);

	float getX() { return worldX; }
	float getY() { return worldY; }


};