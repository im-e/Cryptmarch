#include "Map/Map.h"
#include "Rendering/Camera.h"

bool Map::loadMap(const std::string& path)
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
	//get the world size in pixels by multiplying the tile size by the tilemap
	mapWidth = width * tilePixelSize;
	mapHeight = height * tilePixelSize;
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

void Map::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	//for each tile, draw tile one by one
	//i = slot in map
	for (int i = 0; i < tilemapHeight * tilemapWidth; i++)
	{
		//get tile number
		int tileNumber = tilemapData[i];
		//get the tile definition from the tile number
		//use a reference to avoid making a full copy
		const TileDefinition& tile = tileDefinitions[tileNumber];

		//calculate the x and y (in the tilemap) based on the slot position
		int tilePosX = i % tilemapWidth; //the remainder after dividing is how far along the row you are
		int tilePosY = i / tilemapWidth; //how many whole rows fit before it

		//convert tilemap posx and posy to world space
		int tileWorldX = tilePosX * tilePixelSize;
		int tileWorldY = tilePosY * tilePixelSize;

		//convert to screen coordinates using the camera
		int tileScreenX = cam.worldToScreenX(tileWorldX);
		int tileScreenY = cam.worldToScreenY(tileWorldY);

		//fill in information to draw
		Graphics::DrawTilemapImage(window, *tilesetImage, tilePixelSize, tile.tileXOffset, tile.tileYOffset, tileScreenX, tileScreenY);
	}
}

bool Map::isWalkable(float pixelX, float pixelY)
{
	//convert pixel coordinate into tile grid coordinate
	//map is made up of 32x32 tiles if my character is at x=100 it will be tile 3. 
	int tileX = pixelX / tilePixelSize;
	int tileY = pixelY / tilePixelSize;

	//check if the tile coordinate exists within the map
	if (tileX < 0 || tileX >= tilemapWidth || tileY < 0 || tileY >= tilemapHeight)
	{
		return false; //this means that anything outside the map will act like an invisible wall
	}

	//convert the 2d coordinates to a 1d array index
	int index = tileY * tilemapWidth + tileX;

	//get the tile from the index
	int tileValue = tilemapData[index];
	return tileDefinitions[tileValue].walkable; //check if that tile's definition is walkable and return it
}


bool Map::isBlockedByWall(float posX, float posY, float width, float height) const
{
	//find the edges of the object in world space
	//-1 since 32 wide box from x=0 is 0-31, pixel 32 is the next tile.
	float left = posX; //left edge
	float right = posX + width - 1.0f; //right edge
	float top = posY; //top edge
	float bottom = posY + height - 1.0f; //bottom edge

	//convert pixel coordinates of edges into tile grid coordinates
	//EG:   4
	//    3   5
	//      5
	int leftTile = left / 32; // / 32 converts it into a tile coordinate
	int rightTile = right / 32;
	int topTile = top / 32;
	int bottomTile = bottom / 32;

	//loop through all tiles that the object edges overlap
	for (int y = topTile; y <= bottomTile; ++y) 
	{
		for (int x = leftTile; x <= rightTile; ++x) 
		{
			///check if any part of the box is trying to move out fo the map
			if (x < 0 || x >= tilemapWidth || y < 0 || y >= tilemapHeight)
			{
				return true; //its out of bounds, so its blocked 
			}
			
			//convert the coordinates into a 1d array index
			int index = y * tilemapWidth + x;
			//get the tile value
			int tileValue = tilemapData[index];

			if (!tileDefinitions[tileValue].walkable) //if its walkable
			{
				return true; //return true as it is blocked
			}
		}
	}

	return false; //if player isnt colliding with any tiles allow movement
}