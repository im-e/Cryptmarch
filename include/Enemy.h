#pragma once
#include "Entity.h"


class Enemy : public Entity
{
private:
	Entity* target;


public:

	Enemy(int _health, float _speed, int _x, int _y, const std::string& path, int _tileXOffset, int _tileYOffset, Entity* _target) :
		Entity(_health, _speed, _x, _y, path, _tileXOffset, _tileYOffset)
	{
		target = _target;
	}


	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override
	{
		//could make it so enemies have a random movement, until they get a target and then move to target
		if (target == nullptr) return;
		//subtract the npc position from the player position to give a direction arrow
		float dirX = target->getX() - worldX;
		float dirY = target->getY() - worldY;


		//each enemy computes sqrt every frame (optimisation potential?)
		//work out the length
		float length = sqrt(dirX * dirX + dirY * dirY);

		//normalise to give us a direction vector
		if (length > 0.0f) //if the length is greater than 0
		{
			dirX /= length; //divide the components by it 
			dirY /= length;
		}

		//apply the movement to the enemy's position based on speed and deltaTime
		//use x/yMove to track how far the enemy wants to move
		float xMove = dirX * speed * deltaTime;
		float yMove = dirY * speed * deltaTime;


		//test if the enemy is blocked then move
		//i could have done move-then-undo but i didnt want my position to enter an illegal state
		if (!map.isBlockedByWall(worldX + xMove, worldY, hitboxWidth, hitboxHeight))
		{
			worldX += xMove;
		}

		if (!map.isBlockedByWall(worldX, worldY + yMove, hitboxWidth, hitboxHeight))
		{
			worldY += yMove;
		}

		//clamp the enemies position to the world
		if (worldX < 0.0f)
		{
			worldX = 0.0f;
		}
		else if (worldX > map.getMapWidth() - hitboxWidth)
		{
			worldX = map.getMapWidth() - hitboxWidth;
		}

		if (worldY < 0.0f)
		{
			worldY = 0.0f;
		}
		else if (worldY > map.getMapHeight() - hitboxHeight)
		{
			worldY = map.getMapHeight() - hitboxHeight;
		}

	}

};
