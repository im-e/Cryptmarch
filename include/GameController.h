#pragma once
#include "Entities/Players/Dwarf.h"
#include "Entities/Players/Archer.h"
#include "Entities/Players/Wizard.h"
#include "Entities/Enemies/EnemyTypes.h"
#include "Entities/Enemies/Enemy.h"
#include "Entities/Enemies/EnemyFactory.h"
#include "Entities/EntityManager.h"
#include "Map/Map.h"

enum GameState
{
    MenuState, PlayState, TallyState
};

//what the tally screen should say
enum class TallyResult
{
    GameOver, //all players died
    LevelComplete, //reached the exit, there is another level
    GameComplete //reached the exit of the last level
};

struct GameStats{
    float timeSurvived = 0.0f; //total for the whole run, enemy unlocks use this so later levels start harder
    int enemiesKilled = 0;

    void reset()
    {
        timeSurvived = 0;
        enemiesKilled = 0;
    }
};

class GameController
{
private:
    GameState gameState = MenuState;
    TallyResult tallyResult = TallyResult::GameOver;

    GameStats stats;

    //levels are played in this order
    static const int levelCount = 2;
    inline static const char* levelPaths[levelCount] =
    {
        "resources/maps/map_dungeon.txt",
        "resources/maps/map_catacombs.txt",
    };
    int currentLevel = 0;

    //loads the current level's map and fills the entity manager
    void startLevel(AssetManager& am, EntityManager& em, Map& map)
    {
        em.clear(); //wipe anything from the last level
        map.loadLevel(levelPaths[currentLevel]);

        Vector2f start = map.getPlayerStart();

        //create the player
        //                       maxhp speed x y imX, imY
        EntityStats dwarfStats = { 100, 200, start.x, start.y, 0, 0 };
        EntityStats archerStats = { 100,  200, start.x, start.y, 2, 0 };
        EntityStats wizardStats = { 100,  200, start.x, start.y, 1, 4 };
        Controls playerControls = { VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_SPACE, VK_SHIFT };

        //create the player (only the one we add is created, otherwise the others would leak memory every level)
        Archer* archer = new Archer(archerStats, &am.playerImage, playerControls, &am.itemsImage, 0, 23);
        //Dwarf* dwarf = new Dwarf(dwarfStats, &am.playerImage, playerControls, &am.itemsImage, 3, 3);
        //Wizard* wizard = new Wizard(wizardStats, &am.playerImage, playerControls, &am.itemsImage, 0, 25);
        //add the player to the entity list
        em.addPlayer(archer);

        Vector<Vector2f>& spawnPoints = map.getSpawnerPositions();
        for (int i = 0; i < spawnPoints.getSize(); i++)
        {
            EntityStats spawnerStats = { 100, 0, spawnPoints[i].x, spawnPoints[i].y, 2, 23 };
            em.addSpawner(new Spawner(spawnerStats, &am.mapImage));
        }

        spawnMapObjects(am, em, map);
    }

    //turns the KEY, EXIT, DOOR, PLATE, FAKEPLATE and TRAP tags from the map file into entities
    void spawnMapObjects(AssetManager& am, EntityManager& em, Map& map)
    {
        Vector<MapObject>& objects = map.getMapObjects();
        for (int i = 0; i < objects.getSize(); i++)
        {
            MapObject& object = objects[i];
            float x = object.pos.x;
            float y = object.pos.y;

            switch (object.type)
            {
                case MapObjectType::Key:
                {
                    EntityStats stats = { 1, 0, x, y, 0, 22 }; //gold key in items.png
                    em.addPickup(new Pickup(stats, &am.itemsImage, PickupType::Key, 0, false)); //false = never expires
                    break;
                }
                case MapObjectType::Exit:
                {
                    //closed wooden door, swaps to the open archway (3, 16) when the key is collected
                    EntityStats stats = { 1, 0, x, y, 2, 16 };
                    Door* door = new Door(stats, &am.mapImage, 0, true, 3, 16);
                    door->close(map);
                    em.addDoor(door);
                    break;
                }
                case MapObjectType::Door:
                {
                    //grey framed door, swaps to the open version (5, 16) when its plate is pressed
                    EntityStats stats = { 1, 0, x, y, 4, 16 };
                    Door* door = new Door(stats, &am.mapImage, object.id, false, 5, 16);
                    door->close(map);
                    em.addDoor(door);
                    break;
                }
                case MapObjectType::Plate:
                case MapObjectType::FakePlate:
                {
                    EntityStats stats = { 1, 0, x, y, 9, 16 };
                    bool isFake = (object.type == MapObjectType::FakePlate);
                    em.addPlate(new PressurePlate(stats, &am.mapImage, object.id, isFake));
                    break;
                }
                case MapObjectType::Trap:
                {
                    EntityStats stats = { 1, 0, x, y, 15, 16 };
                    em.addTrap(new SpikeTrap(stats, &am.mapImage));
                    break;
                }
            }
        }
    }

public:

    GameState getState() { return gameState; }

    GameStats& getStats() { return stats; }

    TallyResult getTallyResult() { return tallyResult; }

    int getLevelNumber() { return currentLevel + 1; } //for displaying, levels start at 1 for the player

    //called from the menu
    void startNewGame(AssetManager& am, EntityManager& em, Map& map)
    {
        stats.reset();
        currentLevel = 0;
        startLevel(am, em, map);
        gameState = PlayState;
    }

    //called when the player dies or reaches the exit
    void endLevel(bool reachedExit)
    {
        if (!reachedExit) tallyResult = TallyResult::GameOver;
        else if (currentLevel + 1 < levelCount) tallyResult = TallyResult::LevelComplete;
        else tallyResult = TallyResult::GameComplete;

        gameState = TallyState;
    }

    //called when enter is pressed on the tally screen
    void continueFromTally(AssetManager& am, EntityManager& em, Map& map)
    {
        if (tallyResult == TallyResult::LevelComplete)
        {
            currentLevel++; //move on to the next map, stats carry over
            startLevel(am, em, map);
            gameState = PlayState;
        }
        else
        {
            gameState = MenuState; //died or finished every level
        }
    }
};
