#pragma once
#include "GamesEngineeringBase.h"

class TileDefinition
{
public:

	std::string name; //name of tile - not used but used for designing
	int tileXOffset; //x offset of tile
	int tileYOffset; //y offset of tile
	bool walkable; //if the tile is walkable or not
	int baseTile; //the tile ID drawn to be drawn underneath this one (for transparent tiles) -1 = none
	TileDefinition(std::string tileName, int xOffset, int yOffset, bool isWalkable, int _baseTile = -1) //base tile set to default -1 
	{
		name = tileName;
		tileXOffset = xOffset;
		tileYOffset = yOffset;
		walkable = isWalkable;
		baseTile = _baseTile;
	}
};