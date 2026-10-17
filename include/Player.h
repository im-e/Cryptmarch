#pragma once
#include "GamesEngineeringBase.h"

class Player
{

private:
	GamesEngineeringBase::Image playerImage;
	float playerX = 50.0f;
	float playerY = 100.0f;
	float playerSpeed = 200.0f; // pixels per second
	int playerWidth = 32;
	int playerHeight = 32;

	int keyUp = VK_UP;
	int keyDown = VK_DOWN;
	int keyLeft = VK_LEFT;
	int keyRight = VK_RIGHT;

public:
	bool load(const std::string& path);
	void update(GamesEngineeringBase::Window& window, float deltaTime);
	void draw(GamesEngineeringBase::Window& window);


};