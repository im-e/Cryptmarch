#include "Entities/Entity.h"

void Entity::takeDamage(int damage) {
	//only take damage if iFrames <= 0;
	if (iFrameTimer <= 0)
	{
		int endDamage = (int)(damage * damageTakenMultiplier); //multiply damage by the modifier

		health -= endDamage; //take damage
		if (health < 0) health = 0; //dont let health go below 0
		iFrameTimer = iFrameValue; //reset iFrameTimer to iFrameValue
		hitFlashTimer = hitFlashDuration; //set flash timer to duration
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
	//worldx/y are the top left coordinates of the entity's sprite, to find the middle of our hitbox
	//we have to add the tile size (32) - hitbox size to find the leftover space 
	//split it in half, and then apply it to the x and y offset to keep it central
	//this still returns the top/left coordinates, but the hitbox is central.
	float x = worldX + (tileSize - hitboxWidth) / 2;
	float y = worldY + (tileSize - hitboxHeight) / 2;

	return { x, y, width, height };
}

Vector2f Entity::getCentrePoint() const
{
	float x = worldX + (tileSize / 2.0f);
	float y = worldY + (tileSize / 2.0f);

	return { x, y };
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

	hitFlashTimer -= deltaTime;
	if (hitFlashTimer < 0) hitFlashTimer = 0;
}

void Entity::drawSprite(GamesEngineeringBase::Window& window, Camera& cam, unsigned char tintR, unsigned char tintG, unsigned char tintB, unsigned char tintStrength)
{
	int screenX = cam.worldToScreenX(worldX); //convert worldx to screenx via cam transform
	int screenY = cam.worldToScreenY(worldY);

	if (isFlashing()) //if hit and is flashing - overrides any other tint
	{
		tintR = 255; //red tint
		tintG = 40; 
		tintB = 40; 
		tintStrength = 200; //by changing the input values
	}

	Graphics::DrawTilemapImage(window, *image, tileSize, tileXOffset, tileYOffset, screenX, screenY, tintR, tintG, tintB, tintStrength);
}