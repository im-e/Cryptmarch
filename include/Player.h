#pragma once
#include "GamesEngineeringBase.h"

class Player
{

private:
	//image
	GamesEngineeringBase::Image playerImage;
	int playerTileSize = 24; //tilesize


	//position
	float playerX = 50.0f;
	float playerY = 100.0f;

	//movement
	float playerSpeed = 200.0f; // pixels per second

	//bounding box size
	int playerWidth = 16;
	int playerHeight = 16;

	int keyUp = VK_UP;
	int keyDown = VK_DOWN;
	int keyLeft = VK_LEFT;
	int keyRight = VK_RIGHT;

public:
	bool load(const std::string& path);
	void update(GamesEngineeringBase::Window& window, float deltaTime);
	void draw(GamesEngineeringBase::Window& window);


};