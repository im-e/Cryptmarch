#include "Entities/Door.h"

Door::Door(EntityStats _stats, GamesEngineeringBase::Image* _image, int _id, bool _isExit, int _openTileX, int _openTileY) :
	Entity(_stats, _image)
{
	id = _id;
	isExit = _isExit;
	openTileX = _openTileX;
	openTileY = _openTileY;

	//convert the world position back into a tile position, doors are always placed on exact tiles
	mapTileX = (int)(worldX / tileSize);
	mapTileY = (int)(worldY / tileSize);
}

void Door::close(Map& map)
{
	opened = false;
	map.setTileBlocked(mapTileX, mapTileY, true); //now the player, enemies and projectiles treat it like a wall
}

void Door::open(Map& map)
{
	if (opened) return; //already open, nothing to do

	opened = true;
	map.setTileBlocked(mapTileX, mapTileY, false); //walkable again

	//swap to the open sprite, this is the visual change that tells the player something happened
	tileXOffset = openTileX;
	tileYOffset = openTileY;
}

void Door::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	drawSprite(window, cam);

#ifdef _DEBUG
	AABB hitbox = getHitbox();
	Graphics::DrawRectOutline(window, cam.worldToScreenX(hitbox.x), cam.worldToScreenY(hitbox.y), hitboxWidth, hitboxHeight, 255, 255, 0);
#endif
}
