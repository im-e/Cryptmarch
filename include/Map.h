#pragma once
#include "GamesEngineeringBase.h"
#include "TileDefinition.h"
#include "Graphics.h"
#include <fstream>
#include "Utilities/Logger.h"

class Map
{
private:
	//tileset image
	GamesEngineeringBase::Image tilemapImage;
	bool loadImage(const std::string& path)
	{
		if (tilemapImage.load(path)) {
			return true;
		}
		else {
			Logger::Error("Map Tilemap failed to load at filepath : " + path);
			return false;
		}
	}

	//tile type definitions
	inline static TileDefinition tileDefinitions[2]
	{
					//name xpos ypos walkable
		TileDefinition("Floor", 0, 6, true),
		TileDefinition("Wall", 0, 1, false),
	};

	//map grid 2d array of tile ids
	//1d flat array with index y * width + x
	//map data read from loader
	int* tilemapData = nullptr;
	int tilemapWidth = 0;
	int tilemapHeight = 0;
	int tilemapTileSize = 32;

	bool loadMap(const std::string& path)
	{
		//load values from file
		std::fstream mapFile(path);

		if (!mapFile.is_open())
		{
			Logger::Error("Map Failed to load at: " + path);
			return false;
		}

		int width = 0;
		int height = 0;

		//read and extract width and height from the first line
		mapFile >> width >> height;

		//update width/height
		tilemapWidth = width;
		tilemapHeight = height;
		//allocate array of new map
		tilemapData = new int[tilemapWidth * tilemapHeight];

		//read the data from the file and insert it into the array
		for (int i = 0; i < tilemapWidth * tilemapHeight; i++)
		{
			mapFile >> tilemapData[i];
		}

		mapFile.close();
		Logger::Info("Successfully loaded map: " + path);
		return true;
	}

public:

	Map(const std::string& imagePath, const std::string& mapPath)
	{
		loadImage(imagePath);
		loadMap(mapPath);

	}

	~Map() {
		delete[] tilemapData;
	}

	void draw(GamesEngineeringBase::Window& window)
	{
		//for each tile, draw tile one by one

		//i = slot in map
		for (int i = 0; i < tilemapHeight * tilemapWidth; i++)
		{
			//get tile number
			int tileNumber = tilemapData[i];
			//get the tile definition from the tile number
			//use a reference to avoid making a full copy
			const TileDefinition &tile = tileDefinitions[tileNumber];

			//calculate the x and y (in the tile tilemap) based on the slot position
			int tilePosX = i % tilemapWidth; //the remainder after dividing is how far along the row you are
			int tilePosY = i / tilemapWidth; //integer division counts how many whole rows fit before it

			//convert tilemap posx and posy to draw space
			tilePosX *= tilemapTileSize;
			tilePosY *= tilemapTileSize;

			//fill in information to draw
			Graphics::DrawTilemapImage(window, tilemapImage, tilemapTileSize, tile.tileXOffset, tile.tileYOffset, tilePosX, tilePosY);

		}
	}
};
