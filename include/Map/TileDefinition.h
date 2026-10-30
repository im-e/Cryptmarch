#pragma once
#include "GamesEngineeringBase.h"

class TileDefinition
{
public:

	std::string name;
	int tileXOffset;
	int tileYOffset;
	bool walkable;
	int baseTile; //tile ID drawn underneath this one (for tiles with a transparent background), -1 = none

	//_baseTile has a default value, so old 4 argument definitions still compile
	TileDefinition(std::string tileName, int xOffset, int yOffset, bool isWalkable, int _baseTile = -1)
	{
		name = tileName;
		tileXOffset = xOffset;
		tileYOffset = yOffset;
		walkable = isWalkable;
		baseTile = _baseTile;
	}
};