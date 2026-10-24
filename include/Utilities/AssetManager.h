#pragma once
#include "../GamesEngineeringBase.h"
#include "Logger.h"


class AssetManager {
private:


public:
	GamesEngineeringBase::Image enemyImage;
	GamesEngineeringBase::Image playerImage;
	GamesEngineeringBase::Image hudImage;
	GamesEngineeringBase::Image mapImage;
	GamesEngineeringBase::Image mainMenuImage;
	GamesEngineeringBase::Image tallyImage;
	AssetManager()
	{
		//load in filepaths for images
		if (!enemyImage.load("resources/32rogues/monsters.png")) Logger::Error("Enemy Image failed to load.");
		if(!playerImage.load("resources/32rogues/rogues.png"))	Logger::Error("Player Image failed to load.");
		if (!hudImage.load("resources/ShooterPack/PNG/Interface/Tilemap/tilemap_packed.png")) Logger::Error("Hud Image failed to load.");
		if (!mapImage.load("resources/32rogues/tiles.png"))	Logger::Error("Map Image failed to load.");
		if (!mainMenuImage.load("resources/MainMenu.png"))	Logger::Error("MainMenu Image failed to load.");
		if (!tallyImage.load("resources/Tally.png"))	Logger::Error("Tally Image failed to load.");

	}
};