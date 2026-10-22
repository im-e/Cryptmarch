#include "Entities/Entity.h"

void Entity::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	int screenX = cam.worldToScreenX(worldX);
	int screenY = cam.worldToScreenY(worldY);

	Graphics::DrawTilemapImage(window, *image, tileSize, tileXOffset, tileYOffset, screenX, screenY);

#ifdef _DEBUG

	float entityX, entityY;
	int entityWidth, entityHeight;
	getHitbox(entityX, entityY, entityWidth, entityHeight);

	Graphics::DrawRectOutline(window, (entityX - cam.getX()), (entityY - cam.getY()), entityWidth, entityHeight, 0, 255, 0);                          

#endif
}

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

void Entity::getHitbox(float& x, float& y, int& width, int& height) const
{
	width = hitboxWidth; //set width and height to hitbox height
	height = hitboxHeight;
	//worldx/y are the top left coordinates of the entity, to find the middle
	//we have to add the tile size (32) - hitbox size to find the leftover space
	//split it in half, and then apply it to the x and y offset to keep it central 
	x = worldX + (tileSize - hitboxWidth) / 2;
	y = worldY + (tileSize - hitboxHeight) / 2;
}

bool Entity::checkCollision(const Entity* collider) const
{
	//get the bounding box coordinates for this entity
	float entityX, entityY;
	int entityWidth, entityHeight;
	getHitbox(entityX, entityY, entityWidth, entityHeight);

	//get the bounding box coordinates for the colliding entity
	float colliderX, colliderY;
	int colliderWidth, colliderHeight;
	collider->getHitbox(colliderX, colliderY, colliderWidth, colliderHeight);

	//check for overlap on both axes
	//x axis: entity's left edge is left of colliders right edge && entity's right edge is right of colliders left edge
	bool xOverlap = (entityX < colliderX + colliderWidth) && (entityX + entityWidth > colliderX);
	//y axis: entity's top edge is above colliders bottom edge && entity's bottom edge is below of colliders top edge
	bool yOverlap = (entityY < colliderY + colliderHeight) && (entityY + entityHeight > colliderY);

	//you can easily have horizontal overlap without a collision EG: they are at the same x position but with completely different y levels
	//so its importatnt that there is a 'collision' on both axis
	return xOverlap && yOverlap;

}

void Entity::updateTimers(float deltaTime)
{
	iFrameTimer -= deltaTime;
	if (iFrameTimer < 0) iFrameTimer = 0;
}