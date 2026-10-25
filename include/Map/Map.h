#pragma once
#include "GamesEngineeringBase.h"
#include "TileDefinition.h"
#include "Rendering/Graphics.h"
#include <fstream>
#include "Utilities/Logger.h"

//forward declaration, tells the compiler camera is a class
//so we can use camera& without compiler getting stuck in a loop
class Camera; 

class Map
{
private:

	GamesEngineeringBase::Image* tilesetImage; //tileset image
	bool loadImage(const std::string& path); //loads the image

	int* tilemapData = nullptr; //map grid 1d flat array of tile ids
	int tilemapWidth = 0; //width of tilemap (in tiles)
	int tilemapHeight = 0; //height of tilemap (in tiles)

	int mapWidth = 0; //width of tilemap (in pixels) (map size)
	int mapHeight = 0; //height of tilemap (in pixels) (map size)

	int tilePixelSize = 32; //width + height of each tile in pixels

	//tile type definitions
	inline static TileDefinition tileDefinitions[7]
	{
		//name xpos ypos walkable
		TileDefinition("Background", 0, 7, true),
		TileDefinition("Floor1", 0, 6, true),
		TileDefinition("Floor2", 1, 6, true),
		TileDefinition("Floor3", 2, 6, true),
		TileDefinition("Floor4", 3, 6, true),
		TileDefinition("RockWallVertical", 0, 1, false),
		TileDefinition("RockWallHorizontal", 1, 1, false),
	};

	bool loadMap(const std::string& path); //loads map data

public:

	Map(GamesEngineeringBase::Image* _image, const std::string& mapPath) //constructor
	{
		tilesetImage = _image;
		loadMap(mapPath);
	}

	~Map() //destructor
	{
		delete[] tilemapData;
	}

	void draw(GamesEngineeringBase::Window& window, Camera& cam); //draw the map relative to the camera position

	int getTilemapWidth() { return tilemapWidth; } //returns the width of the map in tiles
	int getTilemapHeight() { return tilemapHeight; } //retuns the height of th map in tiles

	int getMapWidth() { return mapWidth; } //returns the width of the map in pixels
	int getMapHeight() { return mapHeight; }//returns the height of the map in pixels

	bool isWalkable(float pixelX, float pixelY); //returns if a point in 

	bool isBlockedByWall(float posX, float posY, int width, int height) const;


};
