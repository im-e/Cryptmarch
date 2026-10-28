#include "Entities/Spawner.h"

void Spawner::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	drawSprite(window, cam);

#ifdef _DEBUG

	AABB hitbox = getHitbox();

	Graphics::DrawRectOutline(window, cam.worldToScreenX(hitbox.x), cam.worldToScreenY(hitbox.y), hitboxWidth, hitboxHeight, 255, 0, 0);

#endif

}

void Spawner::update(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
	spawnTimer -= deltaTime; //decrease the spawn timer
	if (spawnTimer <= 0) { //if spawn timer is less than 0
		readyToSpawn = true; //set spawn
		spawnTimer = spawnInterval; //set spawn timer to spawn interval
	}
}