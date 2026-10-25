#include "Entities/Entity.h"

void Entity::takeDamage(int damage) {
	//only take damage if iFrames <= 0;
	if (iFrameTimer <= 0)
	{
		health -= damage; //take damage
		if (health < 0) health = 0; //dont let health go below 0
		iFrameTimer = iFrameValue; //reset iFrameTimer to iFrameValue
	}
}

bool Entity::isDead() const {
	if (health <= 0) return true;
	return false;
}


void Entity::setHitboxSize(int width, int height)
{
	hitboxWidth = width;
	hitboxHeight = height;
}

AABB Entity::getHitbox() const
{
	int width = hitboxWidth; //set width and height to hitbox height
	int height = hitboxHeight;
	//worldx/y are the top left coordinates of the entity, to find the middle
	//we have to add the tile size (32) - hitbox size to find the leftover space
	//split it in half, and then apply it to the x and y offset to keep it central 
	float x = worldX + (tileSize - hitboxWidth) / 2;
	float y = worldY + (tileSize - hitboxHeight) / 2;

	return { x, y, width, height };
}

bool Entity::checkEntityCollision(const Entity* collider) const
{
	//get the bounding box coordinates for this entity
	AABB entityBox = getHitbox();

	//get the bounding box coordinates for the colliding entity
	AABB colliderBox = collider->getHitbox();

	return Collisions::checkAABBCollision(entityBox, colliderBox);

}

void Entity::updateTimers(float deltaTime)
{
	iFrameTimer -= deltaTime;
	if (iFrameTimer < 0) iFrameTimer = 0;
}