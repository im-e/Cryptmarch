#pragma once

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
        Player* player = new Player(100, .5f, 200, 400, 400, &am.playerImage, 0, 0, &am.itemsImage, 6, 1);
        //add the player to the entity list
        em.addPlayer(player);

        //create enemies (will be created by spawners soon)
                  //health, speed x y image, offsets
        //add enemies to entity list
        em.addEnemy(new Enemy(100, 20, .25f, 50, 100, 200, &am.enemyImage, 0, 2, player));
        em.addEnemy(new Enemy(100, 20, .25f, 100, 600, 500, &am.enemyImage, 1, 2, player));
        em.addEnemy(new Enemy(100, 20, .25f, 150, 300, 100, &am.enemyImage, 1, 4, player));
        em.addEnemy(new Enemy(100, 20, .25f, 150, 150, 300, &am.enemyImage, 0, 4, player));
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