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
    MenuState, PlayerSelectState, CharacterSelectState, P2CharacterSelectState, ModeSelectState, PlayState, TallyState
};

enum class PlayerMode
{
    SinglePlayer,
    Multiplayer
};

enum class CharacterType
{
    Dwarf,
    Archer,
    Wizard
};

enum class GameModeType
{
    Story,
    Infinite
};

//what the tally screen should say
enum class TallyResult
{
    GameOver, //all players dead
    LevelComplete, //reached the exit, there is another level
    GameComplete //reached the exit of the last level
};

struct GameStats{
    float timeSurvived = 0.0f; //total for the whole run, enemy unlocks use this so later levels start harder
    int enemiesKilled = 0;  //

    void reset()
    {
        timeSurvived = 0;
        enemiesKilled = 0;
    }
};

class GameController
{
private:
    GameState gameState = MenuState; //what state the game is in
    TallyResult tallyResult = TallyResult::GameOver; //the result of the tally screen

    GameStats stats; //game counter stats

    // Menu selection states
    int playerSelectIndex = 0;     //0: single player, 1: multiplayer
    int characterSelectIndex = 0;  //0: dwarf, 1: archer, 2: wizard
    int p2CharacterSelectIndex = 0;  //0: dwarf, 1: archer, 2: wizard
    int modeSelectIndex = 0;       //0: story Mode, 1: infinite Mode

    PlayerMode playerMode = PlayerMode::SinglePlayer; //mode which is chosen
    CharacterType characterType = CharacterType::Dwarf; //character which is chosen
    CharacterType p2CharacterType = CharacterType::Archer; //character which is chosen for player 2
    GameModeType gameMode = GameModeType::Story; //gamemode which is chosen

    //levels are played in this order
    static const int levelCount = 2; //amount of levels we have
    inline static const std::string levelPaths[levelCount] = //string defs for levels
    {
        "resources/maps/map_dungeon.txt",
        "resources/maps/map_catacombs.txt",
    };
    int currentLevel = 0; //what level we are currently on

    //infinite mode
    static const int templateCount = 8;
    inline static const std::string templatePaths[templateCount] =
    {
        "resources/maps/infinite/chunk_0.txt",
        "resources/maps/infinite/chunk_1.txt",
        "resources/maps/infinite/chunk_2.txt",
        "resources/maps/infinite/chunk_3.txt",
        "resources/maps/infinite/chunk_4.txt",
        "resources/maps/infinite/chunk_5.txt",
        "resources/maps/infinite/chunk_6.txt",
        "resources/maps/infinite/chunk_7.txt"
    };

    //helper to spawn player based on character type
    void spawnPlayer(AssetManager& am, EntityManager& em, CharacterType type, float x, float y, Controls controls)
    {
        if (type == CharacterType::Dwarf)
        {
            EntityStats stats = { 100, 200, x, y, 0, 0 };
            em.addPlayer(new Dwarf(stats, &am.playerImage, controls, &am.itemsImage, 3, 3));
        }
        else if (type == CharacterType::Archer)
        {
            EntityStats stats = { 100, 200, x, y, 2, 0 };
            em.addPlayer(new Archer(stats, &am.playerImage, controls, &am.itemsImage, 0, 23));
        }
        else if (type == CharacterType::Wizard)
        {
            EntityStats stats = { 100, 200, x, y, 1, 4 };
            em.addPlayer(new Wizard(stats, &am.playerImage, controls, &am.itemsImage, 0, 25));
        }
    }


    //loads the current level's map and fills the entity manager
    void startLevel(AssetManager& am, EntityManager& em, Map& map)
    {
        em.clear(); //wipe anything from the last level
        if (gameMode == GameModeType::Story)
        {
            map.loadLevel(levelPaths[currentLevel]);

            Vector2f start = map.getPlayerStart();

            Controls p1Controls = { VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, 'L', 'K' };
            Controls p2Controls = { 'W', 'S', 'A', 'D', 'B', 'V' };


            //spawn players based on chosen character type and player mode
            //spawn player 1
            spawnPlayer(am, em, characterType, start.x, start.y, p1Controls);

            //spawn player 2 if multiplayer is enabled
            if (playerMode == PlayerMode::Multiplayer)
            {
                spawnPlayer(am, em, p2CharacterType, start.x + 30, start.y, p2Controls); //spawn p2 with an x offset so no overlap
            }

            //create spawners
            Vector<Vector2f>& spawnPoints = map.getSpawnerPositions();
            for (int i = 0; i < spawnPoints.getSize(); i++)
            {
                EntityStats spawnerStats = { 100, 0, spawnPoints[i].x, spawnPoints[i].y, 2, 23 };
                em.addSpawner(new Spawner(spawnerStats, &am.mapImage));
            }

            //create map objects
            spawnMapObjects(am, em, map);
        }
        else if (gameMode == GameModeType::Infinite)
        {
            map.loadInfinite(templatePaths, templateCount, 0);

            Vector2f start = map.getPlayerStart();
            
            Controls p1Controls = { VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, 'L', 'K' };
            Controls p2Controls = { 'W', 'S', 'A', 'D', 'B', 'V' };
            
            //spawn players based on chosen character type and player mode
            //spawn player 1
            spawnPlayer(am, em, characterType, start.x, start.y, p1Controls);
            
            //spawn player 2 if multiplayer
            if (playerMode == PlayerMode::Multiplayer)
            {
                spawnPlayer(am, em, p2CharacterType, start.x + 30, start.y, p2Controls);
            }

            //dont create spawners as we will do that dynamically
            return;

        }
        
    }

    //turns the KEY, EXIT, DOOR, PLATE, FAKEPLATE and TRAP tags from the map file into entities
    void spawnMapObjects(AssetManager& am, EntityManager& em, Map& map)
    {
        Vector<MapObject>& objects = map.getMapObjects(); //get map objects (list of all objects in the map file
        for (int i = 0; i < objects.getSize(); i++) //for each object
        {
            MapObject& object = objects[i]; //get the object
            float x = object.pos.x; //set x and y pos
            float y = object.pos.y;

            switch (object.type) //based on object type
            {
                case MapObjectType::Key:
                {
                    //everything has 1 health as it needs to stay alive to not be deleted
                    //0 speed so it doesnt move
                    EntityStats stats = { 1, 0, x, y, 0, 22 }; //gold key 
                    em.addPickup(new Pickup(stats, &am.itemsImage, PickupType::Key, 0, false)); //false = to ensure the key never expires
                    break;
                }
                case MapObjectType::Exit:
                {
                    //closed wooden dor, swaps to the open archway (3, 16) when the key is collected
                    EntityStats stats = { 1, 0, x, y, 2, 16 }; //set as closed wooden door
                    Door* door = new Door(stats, &am.mapImage, 0, true, 3, 16); //add the coordinates for the open version
                    door->close(map); //set door as closed
                    em.addDoor(door);
                    break;
                }
                case MapObjectType::Door:
                {
                    //grey door, swaps to the open version (5, 16) when its pressure plate is activated
                    EntityStats stats = { 1, 0, x, y, 4, 16 }; //set as closed framed door
                    Door* door = new Door(stats, &am.mapImage, object.id, false, 5, 16); //set coordinates to open version
                    door->close(map); //set as closed
                    em.addDoor(door);
                    break;
                }
                case MapObjectType::Plate: //for pressure plates and fake plates
                case MapObjectType::FakePlate:
                {
                    EntityStats stats = { 1, 0, x, y, 9, 16 }; //same look for the fake and real plates
                    bool isFake = (object.type == MapObjectType::FakePlate); //if object type is a fake plate, then is fake is true
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
    void setState(GameState state) { gameState = state; }

    GameStats& getStats() { return stats; }

    TallyResult getTallyResult() { return tallyResult; }

    int getLevelNumber() { return currentLevel + 1; } //for displaying, +1 the index

    int getPlayerSelectIndex() const { return playerSelectIndex; }
    void setPlayerSelectIndex(int idx) { playerSelectIndex = idx; }

    int getCharacterSelectIndex() const { return characterSelectIndex; }
    void setCharacterSelectIndex(int idx) { characterSelectIndex = idx; }

    int getP2CharacterSelectIndex() const { return p2CharacterSelectIndex; }
    void setP2CharacterSelectIndex(int idx) { p2CharacterSelectIndex = idx; }

    int getModeSelectIndex() const { return modeSelectIndex; }
    void setModeSelectIndex(int idx) { modeSelectIndex = idx; }

    void confirmPlayerSelection()
    {
        if (playerSelectIndex == 0)
        {
            playerMode = PlayerMode::SinglePlayer;
        }
        else
        {
            playerMode = PlayerMode::Multiplayer;
        }

        gameState = CharacterSelectState;
    }

    void confirmCharacterSelection()
    {
        if (characterSelectIndex == 0) characterType = CharacterType::Dwarf;
        else if (characterSelectIndex == 1) characterType = CharacterType::Archer;
        else characterType = CharacterType::Wizard;

        //if multiplayer go to p2 character select screen
        if (playerMode == PlayerMode::Multiplayer)
        {
            gameState = P2CharacterSelectState;
        }
        else
        {
            gameState = ModeSelectState;
        }
    }

    void confirmP2CharacterSelection()
    {
        if (p2CharacterSelectIndex == 0) p2CharacterType = CharacterType::Dwarf;
        else if (p2CharacterSelectIndex == 1) p2CharacterType = CharacterType::Archer;
        else p2CharacterType = CharacterType::Wizard;

        gameState = ModeSelectState;
    }

    //starts a brand new game based on selections
    void startNewGame(AssetManager& am, EntityManager& em, Map& map)
    {
        gameMode = (modeSelectIndex == 0) ? GameModeType::Story : GameModeType::Infinite;
        stats.reset();
        currentLevel = 0;
        startLevel(am, em, map);
        gameState = PlayState;
    }

    //when the player dies or reaches the exit
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
            currentLevel++;
            startLevel(am, em, map);
            gameState = PlayState;
        }
        else
        {
            gameState = MenuState;
        }
    }
};
