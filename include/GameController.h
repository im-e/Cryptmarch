#pragma once
#include "Entities/Players/Dwarf.h"
#include "Entities/Players/Archer.h"
#include "Entities/Players/Wizard.h"
#include "Entities/Enemies/EnemyTypes.h"
#include "Entities/Enemies/Enemy.h"
#include "Entities/Enemies/EnemyFactory.h"

enum GameState
{
    MenuState, PlayState, TallyState
};

struct GameStats{
    float timeSurvived = 0.0f;
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

    GameStats stats;
    void startGame(AssetManager& am, EntityManager& em)
    {
        em.clear(); //wipe anything from the last run
        stats.reset(); //reset stats
        
        //create the player
        //                       maxhp speed x y imX, imY
        EntityStats dwarfStats = { 100, 200, 400, 400, 0, 0 };
        EntityStats archerStats = { 100,  200, 400, 400, 2, 0 };
        EntityStats wizardStats = { 100,  200, 400, 400, 1, 4 };
        Controls playerControls = { VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_SPACE, VK_SHIFT };

        Dwarf* dwarf = new Dwarf(dwarfStats, &am.playerImage, playerControls, &am.itemsImage, 3, 3);
        Archer* archer = new Archer(archerStats, &am.playerImage, playerControls, &am.itemsImage, 0, 23);
        Wizard* wizard = new Wizard(wizardStats, &am.playerImage, playerControls, &am.itemsImage, 0, 25);
        //add the player to the entity list
        em.addPlayer(wizard);

        EntityStats spawnerStats = { 100, 0, 600, 200, 2, 23 };
        Spawner* spawner = new Spawner(spawnerStats, &am.mapImage);
        em.addSpawner(spawner);
    }

public:

    GameState getState() { return gameState; }

    GameStats& getStats() { return stats; }

    void setState(GameState state, AssetManager& am, EntityManager& em) 
    { 
        gameState = state; 

        if (state == PlayState) startGame(am, em);
    }


};