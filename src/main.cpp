#include "GamesEngineeringBase.h"

#include "Utilities/AssetManager.h"

#include "Utilities/Logger.h"
#include "Utilities/Vector.h"

#include "Entities/Player.h"
#include "Entities/Enemy.h"
#include "Entities/EntityManager.h"

#include "Rendering/Graphics.h"
#include "Rendering/Camera.h"

#include "HUD/FPS.h"
#include "HUD/Status.h"

#include "Map/Map.h"

#include "GameController.h"

int main()
{
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
    EntityManager entityManager;

    //init hud
    FPS fps(&assetManager.hudImage);
    Status status(&assetManager.hudImage);

    //init map
    std::string mapPath = "resources/maps/map_arena.txt";
    Map map(&assetManager.mapImage, mapPath);

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
                if (enterPressed) gameController.setState(PlayState, assetManager, entityManager);
                break;

        }
            case PlayState: {
                //------------------------------ UPDATE -------------------
                entityManager.update(window, map, deltaTime); //update all entities
                int kills = entityManager.cleanupKills(); //cleanup any entities killed

                if (entityManager.getAllPlayersDead()) //check if all players are dead before camera work
                {
                    gameController.setState(TallyState, assetManager, entityManager);
                    break;
                }

                //set the camera to follow the player
                Player* p1 = entityManager.getFirstPlayer();
                cam.follow(window, map, p1 ->getX(), p1->getY(), true);

                //increase stats
                gameController.getStats().timeSurvived += deltaTime;
                gameController.getStats().enemiesKilled += kills;

                //------------------------------ DRAW ----------------------
                map.draw(window, cam);

                entityManager.draw(window, cam);
                fps.draw(window, trueDelta); //draw fps using true delta to enforce true fps readings

                status.draw(window, entityManager.getFirstPlayer()->getHealth());

                
                break;
        }
            case TallyState: {
                //draw tally image
                GameStats& s = gameController.getStats();
                Graphics::DrawString(window, assetManager.hudImage, "TIME  -", 300, 300);
                Graphics::DrawNumber(window, assetManager.hudImage, (int)s.timeSurvived, 550, 300);
                Graphics::DrawString(window, assetManager.hudImage, "KILLS -", 300, 350);
                Graphics::DrawNumber(window, assetManager.hudImage, s.enemiesKilled, 550, 350);
                Graphics::DrawString(window, assetManager.hudImage, "PRESS ENTER TO RETURN", 200, 400);
                if (enterPressed) gameController.setState(MenuState, assetManager, entityManager);
                break;
            }
        }

        //display frame on screen 
        window.present();
    }
    return 0;
}