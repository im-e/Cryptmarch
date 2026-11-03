#include "Map/Map.h"
#include "Rendering/Camera.h"

void Map::unload()
{
	delete[] tilemapData; //delete the data, deleting nullptr is safe so this isnt a problem
	delete[] blockedTiles;
	tilemapData = nullptr;
	blockedTiles = nullptr;
	tilemapWidth = 0;
	tilemapHeight = 0;
	spawnerPositions.clear();
	mapObjects.clear();
	playerStart = { 64.0f, 64.0f };
	infinite = false;
	templateCount = 0;
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

int Map::getTileAt(int tileX, int tileY) const //get the tile shown at x and y pos
{
	if (!infinite) //if not infinite
	{
		if (tileX < 0 || tileX >= tilemapWidth || tileY < 0 || tileY >= tilemapHeight) //check if in bounds
		{
			return -1; //tile is out of bounds of the tilemap
		}
		return tilemapData[tileY * tilemapWidth + tileX]; //else return the tile at index
	}
	else //find the tile inside the infinite map 
	{
		//the infinite map is made up from a formula
		int chunkX = Math::floorDiv(tileX, 16); //floor div to find what chunk we are in
		int chunkY = Math::floorDiv(tileY, 16);
		int localX = Math::positiveModulo(tileX, 16); //positive modulo to find where we are in the chunk
		int localY = Math::positiveModulo(tileY, 16);

		int templateIndex = getTemplateIndex(chunkX, chunkY);

		return templates[templateIndex].tiles[localY * templateTileSize + localX];
	}
}

int Map::getTemplateIndex(int chunkX, int chunkY) const
{
	if (chunkX == 0 && chunkY == 0) //force chunk 0,0 
	{//as template 0 
		return 0; //to use template 0 which is a 'safe' room with no spawners
	}

	unsigned int ux = static_cast<unsigned int>(chunkX); //cast to unsigned int for our hash
	unsigned int uy = static_cast<unsigned int>(chunkY); //so the result is never negative before % templateCount
	unsigned int us = static_cast<unsigned int>(seed);

	//index = hash(chunkX, chunkY, seed) % templateCount
	//Teschner et al. (2003), Optimized Spatial Hashing for Collision Detection
	unsigned int hashVal = (ux * 73856093U) ^ (uy * 19349663U) ^ us;
	return hashVal % templateCount;
}

bool Map::loadInfinite(const std::string paths[], int count, int customSeed)
{
	unload();
	if (count > maxTemplates) count = maxTemplates; //check if count exceeds how many templates we have

	for (int i = 0; i < count; i++) //for each template, we want to load it
	{
		templates[i].spawners.clear(); //clear the spawners for a new load

		std::ifstream file(paths[i]); //open the path to the template
		if (!file.is_open())
		{
			//if it fails to open
			Logger::Error("Map Template Failed to load at: " + paths[i]);
			return false;
		} 

		int w, h; //get width and height
		file >> w >> h; //read from file
		if (w != templateTileSize || h != templateTileSize)
		{			
			Logger::Error("Map Template not 16x16");
			return false; //if they arent 16 x 16 return false
		} 

		for (int y = 0; y < templateTileSize; y++) //go through the template y
		{
			for (int x = 0; x < templateTileSize; x++)  //go through the template x
			{ //for each tile
				int tileId; //get the tile id
				file >> tileId; //read from file
				if (tileId < 0 || tileId >= tileCount) tileId = 13; //if its invalid, set to 13 as a fallback
				templates[i].tiles[y * templateTileSize + x] = tileId; //assign the each tile id to its correct tile inside template.tiles
			}
		}

		std::string tag; //load spawners
		while (file >> tag) //while there is tags to be read
		{
			if (tag == "SPAWNER") //if the tag equals spawners
			{
				float sx, sy; //spawner x, spawner y
				file >> sx >> sy; //read the x and y
				templates[i].spawners.add({ sx, sy }); //add the spawner to the template
			}
		}
		file.close(); //close the file
	}

	templateCount = count; //assign that we are using the chosen amount of templates
	infinite = true; //set infinite to be true
	if (customSeed != 0) seed = customSeed; //if we have a custom seed, use it
	else seed = static_cast<int>(rand()); //else use a random seed
	playerStart = { 256.0f, 256.0f }; //set player start
	return true; //return true to know we successfully loaded the map

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

			if (tag == "KEY") object.type = MapObjectType::Key; //then the object tag
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
			Logger::Error("Unknown tag in map file: " + tag); //catches typos or invalid tags
		}
	}


	mapFile.close();
	Logger::Info("Successfully loaded map: " + path);
	return true;
}

void Map::draw(GamesEngineeringBase::Window& window, Camera& cam)
{
	//loop over the tiles the camera can see
	int firstTileX = Math::floorDiv(cam.getX(), tilePixelSize);
	int lastTileX = Math::floorDiv(cam.getX() + window.getWidth(), tilePixelSize);
	int firstTileY = Math::floorDiv(cam.getY(), tilePixelSize);
	int lastTileY = Math::floorDiv(cam.getY() + window.getHeight(), tilePixelSize);

	//loop through all the tiles
	//loop y first
	for (int y = firstTileY; y <= lastTileY; y++)
	{
		//x second
		for (int x = firstTileX; x <= lastTileX; x++)
		{
			//get the tile id at current position
			int id = getTileAt(x, y);
			//if id is -1 (invalid will have logged), or above our tile count (also invalid)
			if (id == -1 || id >= tileCount)
			{
				continue; //skip the tile
			}

			//convert to screen coordinates using the camera
			int screenX = cam.worldToScreenX(x * tilePixelSize);
			int screenY = cam.worldToScreenY(y * tilePixelSize);

			//if this tile has a transparent background, draw its base tile first so it doesnt sit on black
			int base = tileDefinitions[id].baseTile;
			if (base != -1) //ensure base isnt default (invalid tile)
			{
				Graphics::DrawTilemapImage(window, *tilesetImage, tilePixelSize, tileDefinitions[base].tileXOffset, tileDefinitions[base].tileYOffset, screenX, screenY);
			}

			//draw the tile
			Graphics::DrawTilemapImage(window, *tilesetImage, tilePixelSize, tileDefinitions[id].tileXOffset, tileDefinitions[id].tileYOffset, screenX, screenY);

			// Draw blocked indicator for fixed map doors if applicable
			if (!infinite && blockedTiles)
			{
				int idx = y * tilemapWidth + x;
				if (idx >= 0 && idx < tilemapWidth * tilemapHeight && blockedTiles[idx])
				{
					Graphics::DrawTilemapImage(window, *tilesetImage, tilePixelSize, 2, 16, screenX, screenY); // Shut door sprite
				}
			}
		}
	}
}

bool Map::isWalkable(float pixelX, float pixelY)
{
	//convert pixel coordinate into tile grid coordinate 
	int tileX = Math::floorDiv(static_cast<int>(floor(pixelX)), tilePixelSize); //using floor for negative numbers
	int tileY = Math::floorDiv(static_cast<int>(floor(pixelY)), tilePixelSize);

	//get the id of the tile at tilex and tiley
	int id = getTileAt(tileX, tileY);
	if (id == -1 || id >= tileCount) return false; //check for invalid tile, will treat as out of bounds for now so we want it to act like an invisible wall

	//if its not infinite
	if (!infinite)
	{	//get the index
		int index = tileY * tilemapWidth + tileX;
		//check index if its a blocked tile
		if (blockedTiles[index]) return false;
	}

	//return the walkable status of the tiledefinition
	return tileDefinitions[id].walkable;
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
	//using floor division now for negative coordinates
	int minTileX = Math::floorDiv(static_cast<int>(floor(left)), tilePixelSize);
	int maxTileX = Math::floorDiv(static_cast<int>(floor(right)), tilePixelSize);
	int minTileY = Math::floorDiv(static_cast<int>(floor(top)), tilePixelSize);
	int maxTileY = Math::floorDiv(static_cast<int>(floor(bottom)), tilePixelSize);
	
	//loop through all the tiles that the object overlaps
	for (int tileY = minTileY; tileY <= maxTileY; tileY++)
	{
		for (int tileX = minTileX; tileX <= maxTileX; tileX++)
		{
			//get the tile at current position
			int id = getTileAt(tileX, tileY);
			if (id == -1 || id >= tileCount) return true; //check for invalid tile, will treat as out of bounds for now so we want it to act like an invisible wall

			if (!infinite) //if not infinite
			{ //blockedTiles is nullptr on infinite mode, so we only check on non-infinite mode
				//get the index for blocked tiles
				int index = tileY * tilemapWidth + tileX;
				if (blockedTiles[index]) return true; //if that tile is blocked return true
			}

			//check if this tile is not walkable
			if (!tileDefinitions[id].walkable) return true;
		}
	}
	return false; //else its walkable
}