#include "Entities/PressurePlate.h"

PressurePlate::PressurePlate(EntityStats _stats, GamesEngineeringBase::Image* _image, int _id, bool _isFake) :
	Entity(_stats, _image)
{
	id = _id;
	isFake = _isFake;
	tileXOffset = upTileX; //always start with the raised sprite, so real and fake plates look the same
	tileYOffset = plateTileY;
}

bool PressurePlate::press()
{
	if (pressed) return false; //already down, dont trigger again

	pressed = true;
	tileXOffset = downTileX; //show the pressed sprite

	if (isFake) resetTimer = fakeResetTime; //fake plates reset so they can catch the player again

	return true;
}

void PressurePlate::update(GamesEngineeringBase::Window& window, Map& map, float deltaTime)
{
	if (!isFake || !pressed) return; //real plates stay down forever

	resetTimer -= deltaTime;
	if (resetTimer <= 0.0f)
	{
		pressed = false; //pop back up
		tileXOffset = upTileX;
	}
}

void PressurePlate::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	drawSprite(window, cam);
}
