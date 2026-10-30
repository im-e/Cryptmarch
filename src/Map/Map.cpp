#include "Map/Map.h"
#include "Rendering/Camera.h"

void Map::unload()
{
	delete[] tilemapData; //deleting a nullptr is safe, so this works even if nothing was loaded
	delete[] blockedTiles;
	tilemapData = nullptr;
	blockedTiles = nullptr;
	tilemapWidth = 0;
	tilemapHeight = 0;
	spawnerPositions.clear();
	mapObjects.clear();
	playerStart = { 64.0f, 64.0f };
}

bool Map::loadLevel(const std::string& path)
{
	unload(); //throw away the previous level first, otherwise we leak memory and keep old spawners
	return loadMap(path);
}

void Map::setTileBlocked(int tileX, int tileY, bool blocked)
{
	if (tileX < 0 || tileX >= tilemapWidth || tileY < 0 || tileY >= tilemapHeight) return; //outside the map
	blockedTiles[tileY * tilemapWidth + tileX] = blocked;
}

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
	//allocate the blocked array, the () at the end sets every value to false
	blockedTiles = new bool[tilemapWidth * tilemapHeight]();

	//read the data from the file and insert it into the array
	for (int i = 0; i < tilemapWidth * tilemapHeight; i++)
	{
		mapFile >> tilemapData[i];

		//protect against typos in the map file, an unknown ID would read outside tileDefinitions and crash
		if (tilemapData[i] < 0 || tilemapData[i] >= tileCount)
		{
			Logger::Error("Invalid tile ID " + std::to_string(tilemapData[i]) + " at slot " + std::to_string(i) + " in " + path);
			tilemapData[i] = 13; //replace with GreyFloorBlank so the game can still run
		}
	}

	//entities spawned via tilemap
	std::string tag;
	while (mapFile >> tag) //>> will return false when nothing is left, so the loop stops
	{
		if (tag == "SPAWNER")
		{
			int tileX, tileY;
			mapFile >> tileX >> tileY;
			spawnerPositions.add({ (float)(tileX * tilePixelSize), (float)(tileY * tilePixelSize) });
		}
		else if (tag == "PLAYER")
		{
			int tileX, tileY;
			mapFile >> tileX >> tileY;
			playerStart = { (float)(tileX * tilePixelSize), (float)(tileY * tilePixelSize) };
		}
		else if (tag == "KEY" || tag == "EXIT" || tag == "DOOR" || tag == "PLATE" || tag == "FAKEPLATE" || tag == "TRAP")
		{
			//all of these start with a tile position
			int tileX, tileY;
			mapFile >> tileX >> tileY;

			MapObject object;
			object.pos = { (float)(tileX * tilePixelSize), (float)(tileY * tilePixelSize) };

			if (tag == "KEY") object.type = MapObjectType::Key;
			else if (tag == "EXIT") object.type = MapObjectType::Exit;
			else if (tag == "FAKEPLATE") object.type = MapObjectType::FakePlate;
			else if (tag == "TRAP") object.type = MapObjectType::Trap;
			else
			{
				//DOOR and PLATE have an extra number: the id that links them together
				object.type = (tag == "DOOR") ? MapObjectType::Door : MapObjectType::Plate;
				mapFile >> object.id;
			}

			mapObjects.add(object);
		}
		else
		{
			Logger::Error("Unknown tag in map file: " + tag); //catches typos like SPAWNR
		}
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

		//if this tile has a transparent background, draw its base tile first so it doesnt sit on black
		if (tile.baseTile >= 0)
		{
			const TileDefinition& base = tileDefinitions[tile.baseTile];
			Graphics::DrawTilemapImage(window, *tilesetImage, tilePixelSize, base.tileXOffset, base.tileYOffset, tileScreenX, tileScreenY);
		}

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

	if (blockedTiles[index]) return false; //a closed door is on this tile

	//get the tile from the index
	int tileValue = tilemapData[index];
	return tileDefinitions[tileValue].walkable; //check if that tile's definition is walkable and return it
}


bool Map::isBlockedByWall(float posX, float posY, int width, int height) const
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

			if (blockedTiles[index]) //a closed door is on this tile
			{
				return true;
			}
		}
	}

	return false; //if player isnt colliding with any tiles allow movement
}