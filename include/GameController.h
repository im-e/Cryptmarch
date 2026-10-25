#pragma once

#include "Entities/Players/Dwarf.h"

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
        em.clear(); //wipe anything from the last rign
        stats.reset(); //reset stats
        
        //create the player
        //                        health maxhp iframes speed x y imX, imY
        EntityStats playerStats = { 100, 100, .5f, 200, 400, 400, 0, 0 };
        Controls playerControls = { VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_SPACE, VK_SHIFT };

        Dwarf* dwarf = new Dwarf(playerStats, &am.playerImage, playerControls, &am.itemsImage, 3, 3);
        //add the player to the entity list
        em.addPlayer(dwarf);

        //create enemies (will be created by spawners soon)
        //               health maxhp iframes speed x y imX, imY
        EntityStats e1 = { 100, 100, .25f, 50, 100, 200, 0, 2 };
        EntityStats e2 = { 100, 100, .25f, 100, 600, 500, 1, 2 };
        EntityStats e3 = { 100, 100, .25f, 150, 300, 100, 1, 4 };
        EntityStats e4 = { 100, 100, .25f, 150, 150, 300, 0, 2 };

        em.addEnemy(new Enemy(e1,&am.enemyImage, dwarf, 20));
        em.addEnemy(new Enemy(e2,&am.enemyImage, dwarf, 20));
        em.addEnemy(new Enemy(e3,&am.enemyImage, dwarf, 20));
        em.addEnemy(new Enemy(e4,&am.enemyImage, dwarf, 20));
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