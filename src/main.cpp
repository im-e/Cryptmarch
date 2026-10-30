#include "GamesEngineeringBase.h"
#include <cstring> //strlen

#include "Utilities/AssetManager.h"

#include "Utilities/Logger.h"
#include "Utilities/Vector.h"

#include "Entities/Players/Player.h"
#include "Entities/Enemies/Enemy.h"
#include "Entities/EntityManager.h"

#include "Rendering/Graphics.h"
#include "Rendering/Camera.h"

#include "HUD/FPS.h"
#include "HUD/Status.h"

#include "Map/Map.h"

#include "GameController.h"

int main()
{
    srand(time(nullptr)); //random seed for time

    //constant definitions for calculations
    const int windowWidth = 1024;
    const int windowHeight = 768;

    //create the window
    GamesEngineeringBase::Window window;
    //set height and width and give it a name
    window.create(windowWidth, windowHeight, "Cryptmarch");

    //create a timer
    GamesEngineeringBase::Timer timer;

    //create the gameController
    GameController gameController;

    //create the asset and entity managers
    AssetManager assetManager;
    EntityManager entityManager(assetManager, &gameController.getStats().timeSurvived);

    //init hud
    FPS fps(&assetManager.hudImage);
    Status status(&assetManager.hudImage);

    //init map, GameController loads the level file when a game starts
    Map map(&assetManager.mapImage);

    //init camera
    Camera cam;

    //input helper
    bool enterWasDown = false;

    //game loop
    bool running = true;

    while (running)
    {
        if (window.keyPressed(VK_ESCAPE)) break; //if esc is pressed at any time exit the game

        //----------------- DELTA TIME -----------------------
        float deltaTime = timer.dt(); //get time since last frame
        float trueDelta = deltaTime; //get the true delta time for fps measurement
        //cap delta time to prevent unintended behaviour during freezes
        if (deltaTime > 0.05f) //check for very long frames
        {
            deltaTime = 0.05f; //cap the delta time to be shorter
        } //trade-off being that the game might run in slow mo during low frames


        //true only on the frame enter goes down
        bool enterDown = window.keyPressed(VK_RETURN);
        bool enterPressed = enterDown && !enterWasDown;
        enterWasDown = enterDown;

        //clear window for next frame rendering
        window.clear();


        switch (gameController.getState())
        {
            case MenuState: {

                //draw main menu image
                Graphics::DrawImage(window, assetManager.mainMenuImage, 0, 0);

                //check if enter is pressed
                if (enterPressed) gameController.startNewGame(assetManager, entityManager, map);
                break;

        }
            case PlayState: {
                //------------------------------ UPDATE -------------------
                entityManager.update(window, map, deltaTime); //update all entities
                int kills = entityManager.cleanupKills(); //cleanup any entities killed
                gameController.getStats().enemiesKilled += kills; //count kills before leaving the level

                if (entityManager.getAllPlayersDead()) //check if all players are dead before camera work
                {
                    gameController.endLevel(false); //game over
                    break;
                }

                if (entityManager.getLevelComplete()) //walked through the open exit
                {
                    gameController.endLevel(true); //level complete
                    break;
                }

                //set the camera to follow the player
                Player* p1 = entityManager.getFirstPlayer();
                cam.follow(window, map, p1 ->getX(), p1->getY(), true);

                //increase stats
                gameController.getStats().timeSurvived += deltaTime;

                //------------------------------ DRAW ----------------------
                map.draw(window, cam);

                entityManager.draw(window, cam);
                fps.draw(window, trueDelta); //draw fps using true delta to enforce true fps readings

                Player* player = entityManager.getFirstPlayer();
                status.draw(window, player->getHealth(), player->getMaxHealth(), player->getMana(), player->getMaxMana());

                //show the key under the mana bar once it has been picked up
                if (entityManager.getHasKey())
                {
                    Graphics::DrawTilemapImage(window, assetManager.itemsImage, 32, 0, 22, windowWidth - 40, 72);
                }

                
                break;
        }
            case TallyState: {
                //draw tally image
                GameStats& s = gameController.getStats();
                Graphics::DrawImage(window,  assetManager.tallyImage, 0, 0);

                //title depends on how the level ended
                const char* title = "GAME OVER";
                const char* prompt = "PRESS ENTER TO RETURN";
                if (gameController.getTallyResult() == TallyResult::LevelComplete)
                {
                    title = "LEVEL COMPLETE";
                    prompt = "PRESS ENTER TO CONTINUE";
                }
                else if (gameController.getTallyResult() == TallyResult::GameComplete)
                {
                    title = "YOU ESCAPED";
                }

                //centre the title: each letter is 32 pixels wide
                int titleX = (windowWidth - (int)strlen(title) * 32) / 2;
                Graphics::DrawString(window, assetManager.hudImage, title, titleX, 220);

                Graphics::DrawString(window, assetManager.hudImage, "LEVEL -", 300, 300);
                Graphics::DrawNumber(window, assetManager.hudImage, gameController.getLevelNumber(), 550, 300);
                Graphics::DrawString(window, assetManager.hudImage, "TIME  -", 300, 350);
                Graphics::DrawNumber(window, assetManager.hudImage, (int)s.timeSurvived, 550, 350);
                Graphics::DrawString(window, assetManager.hudImage, "KILLS -", 300, 400);
                Graphics::DrawNumber(window, assetManager.hudImage, s.enemiesKilled, 550, 400);
                Graphics::DrawString(window, assetManager.hudImage, prompt, (windowWidth - (int)strlen(prompt) * 32) / 2, 470);
                if (enterPressed) gameController.continueFromTally(assetManager, entityManager, map);
                break;
            }
        }

        //display frame on screen 
        window.present();
    }
    return 0;
}