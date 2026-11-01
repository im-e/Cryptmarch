#include "Entities/MapObjects/SpikeTrap.h"

SpikeTrap::SpikeTrap(EntityStats _stats, GamesEngineeringBase::Image* _image) :
	Entity(_stats, _image)
{
	timer = downTime; //start safe
	tileXOffset = downTileX;
	tileYOffset = trapTileY;
}

void SpikeTrap::update(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
	timer -= deltaTime;
	if (timer > 0.0f) return; //if still in the current phase dont do anything

	spikesUp = !spikesUp; //otherwise change between up and down

	if (spikesUp) //if spikes are up
	{
		timer = upTime; //set to uptime
		tileXOffset = upTileX; //change to up tile
		hasHit = false; //new set of spikes, can hit again
	}
	else
	{
		timer = downTime; //else set do down time
		tileXOffset = downTileX; //change to down tile
	}
}

void SpikeTrap::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	drawSprite(window, cam);
}
