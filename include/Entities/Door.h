#pragma once
#include "Entity.h"

//a door that blocks its tile until it is opened
//exit doors are opened by the key, other doors are opened by the pressure plate with the same id
class Door : public Entity
{
private:
	int id; //links this door to a pressure plate (unused for exit doors)
	bool isExit; //true for the level exit
	bool opened = false; //starts closed

	//sprite position in tiles.png for the open version
	//the closed version is the normal tileXOffset/tileYOffset passed in through EntityStats
	int openTileX;
	int openTileY;

	//which map tile this door sits on, so we can block and unblock it
	int mapTileX;
	int mapTileY;

public:
	Door(EntityStats _stats, GamesEngineeringBase::Image* _image, int _id, bool _isExit, int _openTileX, int _openTileY);

	void close(Map& map); //blocks the tile underneath
	void open(Map& map); //unblocks the tile and swaps to the open sprite

	bool isOpen() const { return opened; }
	bool getIsExit() const { return isExit; }
	int getId() const { return id; }

	void update(GamesEngineeringBase::Window& window, Map& map, float deltaTime) override {}
	void draw(GamesEngineeringBase::Window& window, Camera& cam) override;
};
