#pragma once
#include "Entity.h"

//fake and real pressure plates
class PressurePlate : public Entity
{
private:
	int id; //doors with the same id open when this is pressed
	bool isFake; //true = arrow trap
	bool pressed = false; //if it has been pressed

	float resetTimer = 0.0f; //counts down until the plate can trigger again only for fake plates
	const float fakeResetTime = 1.5f; //reset time for the fake plates

	//sprite positions
	const int upTileX = 9; 
	const int downTileX = 10;
	const int plateTileY = 16;

public:
	PressurePlate(EntityStats _stats, GamesEngineeringBase::Image* _image, int _id, bool _isFake);

	bool press(); //returns true only on the frame the plate goes down

	bool getIsFake() const { return isFake; } 
	int getId() const { return id; }

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override;
	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;
};
