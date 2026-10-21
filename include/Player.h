#pragma once
#include "GamesEngineeringBase.h"
#include "Entity.h"

class Player : public Entity
{

private:
	int keyUp = VK_UP;
	int keyDown = VK_DOWN;
	int keyLeft = VK_LEFT;
	int keyRight = VK_RIGHT;

public:
	Player(int _health, int _speed, float _xPos, float _yPos, const std::string& path, int _tileXOffset, int _tileYOffset) : 
		Entity(_health, _speed, _xPos, _yPos, path, _tileXOffset, _tileYOffset)
	{
	}

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;

};