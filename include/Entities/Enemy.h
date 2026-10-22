#pragma once
#include "Entity.h"


class Enemy : public Entity
{
private:
	Entity* target;
	int contactDamage;

public:

	Enemy(int _health, int _damage, float _iFrames, float _speed, int _x, int _y, GamesEngineeringBase::Image* _image, int _tileXOffset, int _tileYOffset, Entity* _target) :
		Entity(_health, _iFrames, _speed, _x, _y, _image, _tileXOffset, _tileYOffset)
	{
		contactDamage = _damage;
		target = _target;
	}

	int getDamage() const { return contactDamage; }


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

		//pull hitbox origin instead of worldX/Y
		//h for hitbox, had to use this naming convention because of private variables of the same name
		float hX, hY;
		int hWidth, hHeight;
		getHitbox(hX, hY, hWidth, hHeight);


		//test if the enemy is blocked then move
		//i could have done move-then-undo but i didnt want my position to enter an illegal state
		if (!map.isBlockedByWall(hX + xMove, hY, hWidth, hHeight))
		{
			worldX += xMove;
		}

		getHitbox(hX, hY, hWidth, hHeight); //update hitbox after move

		if (!map.isBlockedByWall(hX, hY + yMove, hWidth, hHeight))
		{
			worldY += yMove;
		}

		getHitbox(hX, hY, hWidth, hHeight); //update hitbox after move again

		//clamp the enemy's position to the world using hitbox
		if (hX < 0.0f) //left
		{
			worldX = 0.0f;
		}
		else if (hX > map.getMapWidth() - hWidth) //right
		{
			worldX = map.getMapWidth() - hWidth;
		}
		if (hY < 0.0f) //top
		{
			worldY = 0.0f;
		}
		else if (hY > map.getMapHeight() - hHeight) //bottom
		{
			worldY = map.getMapHeight() - hHeight;
		}

		//iFrame Timer
		updateTimers(deltaTime);

	}

};
