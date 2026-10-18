#pragma once
#include "GamesEngineeringBase.h"

class TileDefinition
{
public:

	std::string name;
	int tileXOffset;
	int tileYOffset;
	bool walkable;

	TileDefinition(std::string tileName, int xOffset, int yOffset, bool isWalkable)
	{
		name = tileName;
		tileXOffset = xOffset;
		tileYOffset = yOffset;
		walkable = isWalkable;
	}
};