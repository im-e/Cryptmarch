#include "Entities/SpikeTrap.h"

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
	if (timer > 0.0f) return; //still in the current phase

	spikesUp = !spikesUp; //flip between up and down

	if (spikesUp)
	{
		timer = upTime;
		tileXOffset = upTileX;
		hasHit = false; //new set of spikes, can hit again
	}
	else
	{
		timer = downTime;
		tileXOffset = downTileX;
	}
}

void SpikeTrap::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	drawSprite(window, cam);
}
