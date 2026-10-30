#pragma once
#include "GamesEngineeringBase.h"
#include "TileDefinition.h"
#include "Rendering/Graphics.h"
#include <fstream>
#include "Utilities/Logger.h"
#include "Utilities/Vector.h"
#include "Utilities/Vector2f.h"

//objects placed by tags in the map file, GameController turns these into entities
enum class MapObjectType { Key, Exit, Door, Plate, FakePlate, Trap };

struct MapObject
{
	MapObjectType type = MapObjectType::Key;
	Vector2f pos = { 0.0f, 0.0f }; //world position in pixels
	int id = 0; //links DOOR and PLATE tags together
};

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

	Vector<Vector2f> spawnerPositions; //world positions of spawners read from the file
	Vector2f playerStart = { 64.0f, 64.0f }; //fallback if the file has no PLAYER line
	Vector<MapObject> mapObjects; //key, exit, doors, plates and traps read from the file

	//one bool per tile, true = blocked by something that isnt part of the tilemap (a closed door)
	//kept separate from tilemapData so a door can open again without changing the tile underneath it
	bool* blockedTiles = nullptr;

	void unload(); //frees the current level so a new one can be loaded


	//tile type definitions for resources/32rogues/tiles.png
	//ordered the same way as the tileset: row by row, left to right (see resources/32rogues/tile_ids.png)
	//the number in /* */ is the tile ID you type in a map file
	//changing the order renumbers every tile, so any existing map files would need regenerating
	//tileCount must match the number of entries, the compiler errors if it doesnt
	static const int tileCount = 128;
	inline static TileDefinition tileDefinitions[tileCount]
	{
		//         name  xpos(column)  ypos(row)  walkable  baseTile(optional)
		//WALLS - Top = solid block seen from above, Side = wall face (put it directly above a floor tile)
		/*  0*/ TileDefinition("DirtWallTop", 0, 0, false),
		/*  1*/ TileDefinition("DirtWallSide", 1, 0, false),
		/*  2*/ TileDefinition("RoughStoneWallTop", 0, 1, false),
		/*  3*/ TileDefinition("RoughStoneWallSide", 1, 1, false),
		/*  4*/ TileDefinition("StoneBrickWallTop", 0, 2, false),
		/*  5*/ TileDefinition("StoneBrickWallSide1", 1, 2, false),
		/*  6*/ TileDefinition("StoneBrickWallSide2", 2, 2, false),
		/*  7*/ TileDefinition("IgneousWallTop", 0, 3, false),
		/*  8*/ TileDefinition("IgneousWallSide", 1, 3, false),
		/*  9*/ TileDefinition("LargeStoneWallTop", 0, 4, false),
		/* 10*/ TileDefinition("LargeStoneWallSide", 1, 4, false),
		/* 11*/ TileDefinition("CatacombWallTop", 0, 5, false),
		/* 12*/ TileDefinition("CatacombWallSide", 1, 5, false),
		//FLOORS - Blank / 1-3 have a background, Overlay versions are transparent and drawn on GreyFloorBlank
		/* 13*/ TileDefinition("GreyFloorBlank", 0, 6, true),
		/* 14*/ TileDefinition("FloorStone1", 1, 6, true),
		/* 15*/ TileDefinition("FloorStone2", 2, 6, true),
		/* 16*/ TileDefinition("FloorStone3", 3, 6, true),
		/* 17*/ TileDefinition("FloorStone1Overlay", 4, 6, true, 13),
		/* 18*/ TileDefinition("FloorStone2Overlay", 5, 6, true, 13),
		/* 19*/ TileDefinition("FloorStone3Overlay", 6, 6, true, 13),
		/* 20*/ TileDefinition("PurpleFloorBlank", 0, 7, true),
		/* 21*/ TileDefinition("Grass1", 1, 7, true),
		/* 22*/ TileDefinition("Grass2", 2, 7, true),
		/* 23*/ TileDefinition("Grass3", 3, 7, true),
		/* 24*/ TileDefinition("Grass1Overlay", 4, 7, true, 13),
		/* 25*/ TileDefinition("Grass2Overlay", 5, 7, true, 13),
		/* 26*/ TileDefinition("Grass3Overlay", 6, 7, true, 13),
		/* 27*/ TileDefinition("Dirt1", 1, 8, true),
		/* 28*/ TileDefinition("Dirt2", 2, 8, true),
		/* 29*/ TileDefinition("Dirt3", 3, 8, true),
		/* 30*/ TileDefinition("Dirt1Overlay", 4, 8, true, 13),
		/* 31*/ TileDefinition("Dirt2Overlay", 5, 8, true, 13),
		/* 32*/ TileDefinition("Dirt3Overlay", 6, 8, true, 13),
		/* 33*/ TileDefinition("StoneFloor1", 1, 9, true),
		/* 34*/ TileDefinition("StoneFloor2", 2, 9, true),
		/* 35*/ TileDefinition("StoneFloor3", 3, 9, true),
		/* 36*/ TileDefinition("StoneFloor1Overlay", 4, 9, true, 13),
		/* 37*/ TileDefinition("StoneFloor2Overlay", 5, 9, true, 13),
		/* 38*/ TileDefinition("StoneFloor3Overlay", 6, 9, true, 13),
		/* 39*/ TileDefinition("Bone1", 1, 10, true),
		/* 40*/ TileDefinition("Bone2", 2, 10, true),
		/* 41*/ TileDefinition("Bone3", 3, 10, true),
		/* 42*/ TileDefinition("Bone1Overlay", 4, 10, true, 13),
		/* 43*/ TileDefinition("Bone2Overlay", 5, 10, true, 13),
		/* 44*/ TileDefinition("Bone3Overlay", 6, 10, true, 13),
		/* 45*/ TileDefinition("RedFloorBlank", 0, 11, true),
		/* 46*/ TileDefinition("RedStoneFloor1", 1, 11, true),
		/* 47*/ TileDefinition("RedStoneFloor2", 2, 11, true),
		/* 48*/ TileDefinition("RedStoneFloor3", 3, 11, true),
		/* 49*/ TileDefinition("RedStoneFloor1Overlay", 4, 11, true, 13),
		/* 50*/ TileDefinition("RedStoneFloor2Overlay", 5, 11, true, 13),
		/* 51*/ TileDefinition("RedStoneFloor3Overlay", 6, 11, true, 13),
		/* 52*/ TileDefinition("BlueFloorBlank", 0, 12, true),
		/* 53*/ TileDefinition("BlueStoneFloor1", 1, 12, true),
		/* 54*/ TileDefinition("BlueStoneFloor2", 2, 12, true),
		/* 55*/ TileDefinition("BlueStoneFloor3", 3, 12, true),
		/* 56*/ TileDefinition("GreenFloorBlank", 0, 13, true),
		/* 57*/ TileDefinition("GreenDirt1", 1, 13, true),
		/* 58*/ TileDefinition("GreenDirt2", 2, 13, true),
		/* 59*/ TileDefinition("GreenDirt3", 3, 13, true),
		/* 60*/ TileDefinition("GreenGrass1", 1, 14, true),
		/* 61*/ TileDefinition("GreenGrass2", 2, 14, true),
		/* 62*/ TileDefinition("GreenGrass3", 3, 14, true),
		/* 63*/ TileDefinition("BrownFloorBlank", 0, 15, true),
		/* 64*/ TileDefinition("BrownBones1", 1, 15, true),
		/* 65*/ TileDefinition("BrownBones2", 2, 15, true),
		/* 66*/ TileDefinition("BrownBones3", 3, 15, true),
		//DOORS AND DUNGEON FEATURES - decoration only for now
		/* 67*/ TileDefinition("Door1", 0, 16, false, 13),
		/* 68*/ TileDefinition("Door2", 1, 16, false, 13),
		/* 69*/ TileDefinition("FramedDoor1Shut", 2, 16, false, 13),
		/* 70*/ TileDefinition("FramedDoor1Open", 3, 16, true, 13),
		/* 71*/ TileDefinition("FramedDoor2Shut", 4, 16, false, 13),
		/* 72*/ TileDefinition("FramedDoor2Open", 5, 16, true, 13),
		/* 73*/ TileDefinition("GratedDoor", 6, 16, false, 13),
		/* 74*/ TileDefinition("StairsDown", 7, 16, true, 13),
		/* 75*/ TileDefinition("StairsUp", 8, 16, true, 13),
		/* 76*/ TileDefinition("PressurePlateUp", 9, 16, true, 13),
		/* 77*/ TileDefinition("PressurePlateDown", 10, 16, true, 13),
		/* 78*/ TileDefinition("Chute", 11, 16, false, 13),
		/* 79*/ TileDefinition("Pit", 12, 16, false, 13),
		/* 80*/ TileDefinition("TrapDoor", 13, 16, true, 13),
		/* 81*/ TileDefinition("Pentagram", 14, 16, true, 13),
		/* 82*/ TileDefinition("SpikesDown", 15, 16, true, 13),
		/* 83*/ TileDefinition("SpikesUp", 16, 16, true, 13),
		//PROPS
		/* 84*/ TileDefinition("ChestClosed", 0, 17, false, 13),
		/* 85*/ TileDefinition("ChestOpen", 1, 17, false, 13),
		/* 86*/ TileDefinition("JarClosed", 2, 17, false, 13),
		/* 87*/ TileDefinition("JarOpen", 3, 17, false, 13),
		/* 88*/ TileDefinition("Barrel", 4, 17, false, 13),
		/* 89*/ TileDefinition("OreSack", 5, 17, false, 13),
		/* 90*/ TileDefinition("LogPile", 6, 17, false, 13),
		/* 91*/ TileDefinition("LargeRock1", 0, 18, false, 13),
		/* 92*/ TileDefinition("LargeRock2", 1, 18, false, 13),
		//PLANTS
		/* 93*/ TileDefinition("Buckwheat", 0, 19, true, 13),
		/* 94*/ TileDefinition("Flax", 1, 19, true, 13),
		/* 95*/ TileDefinition("PapyrusSedge", 2, 19, true, 13),
		/* 96*/ TileDefinition("Kenaf", 3, 19, true, 13),
		/* 97*/ TileDefinition("Ramie", 4, 19, true, 13),
		/* 98*/ TileDefinition("Jute", 5, 19, true, 13),
		/* 99*/ TileDefinition("Rice", 6, 19, true, 13),
		/*100*/ TileDefinition("Wheat", 7, 19, true, 13),
		/*101*/ TileDefinition("Maize", 8, 19, true, 13),
		/*102*/ TileDefinition("Amaranth", 9, 19, true, 13),
		/*103*/ TileDefinition("Quinoa", 10, 19, true, 13),
		/*104*/ TileDefinition("BitterVetch", 11, 19, true, 13),
		/*105*/ TileDefinition("Sorghum", 12, 19, true, 13),
		/*106*/ TileDefinition("RedSpinach", 13, 19, true, 13),
		/*107*/ TileDefinition("Cotton", 14, 19, true, 13),
		/*108*/ TileDefinition("Alfalfa", 15, 19, true, 13),
		//MUSHROOMS
		/*109*/ TileDefinition("SmallMushrooms", 0, 20, true, 13),
		/*110*/ TileDefinition("LargeMushroom", 1, 20, false, 13),
		//CORPSES
		/*111*/ TileDefinition("Corpse1", 0, 21, true, 13),
		/*112*/ TileDefinition("Corpse2", 1, 21, true, 13),
		//BLOOD AND SLIME
		/*113*/ TileDefinition("BloodSpatter1", 0, 22, true, 13),
		/*114*/ TileDefinition("BloodSpatter2", 1, 22, true, 13),
		/*115*/ TileDefinition("SlimeSmall", 2, 22, true, 13),
		/*116*/ TileDefinition("SlimeLarge", 3, 22, true, 13),
		//COFFINS
		/*117*/ TileDefinition("CoffinClosed", 0, 23, false, 13),
		/*118*/ TileDefinition("CoffinAjar", 1, 23, false, 13),
		/*119*/ TileDefinition("CoffinOpen", 2, 23, false, 13),
		/*120*/ TileDefinition("SarcophagusClosed", 3, 23, false, 13),
		/*121*/ TileDefinition("SarcophagusAjar", 4, 23, false, 13),
		/*122*/ TileDefinition("SarcophagusOpen", 5, 23, false, 13),
		//TREES - TallTreeTop goes directly above TallTreeBottom
		/*123*/ TileDefinition("TallTreeTop", 3, 24, false, 13),
		/*124*/ TileDefinition("Sapling", 0, 25, true, 13),
		/*125*/ TileDefinition("SmallTree", 1, 25, false, 13),
		/*126*/ TileDefinition("Tree", 2, 25, false, 13),
		/*127*/ TileDefinition("TallTreeBottom", 3, 25, false, 13),
	};

	bool loadMap(const std::string& path); //loads map data

public:

	Map(GamesEngineeringBase::Image* _image) //constructor, the level is loaded later with loadLevel
	{
		tilesetImage = _image;
	}

	~Map() //destructor
	{
		unload();
	}

	bool loadLevel(const std::string& path); //clears the old level and loads a new one

	void draw(GamesEngineeringBase::Window& window, Camera& cam); //draw the map relative to the camera position

	int getTilemapWidth() { return tilemapWidth; } //returns the width of the map in tiles
	int getTilemapHeight() { return tilemapHeight; } //retuns the height of th map in tiles

	int getMapWidth() { return mapWidth; } //returns the width of the map in pixels
	int getMapHeight() { return mapHeight; }//returns the height of the map in pixels

	Vector<Vector2f>& getSpawnerPositions() { return spawnerPositions; } //returns spawner positions
	Vector2f getPlayerStart() { return playerStart; } //returns start point of player
	Vector<MapObject>& getMapObjects() { return mapObjects; } //returns key, exit, doors, plates and traps

	void setTileBlocked(int tileX, int tileY, bool blocked); //used by doors

	bool isWalkable(float pixelX, float pixelY); //returns if a point in 

	bool isBlockedByWall(float posX, float posY, int width, int height) const;


};
