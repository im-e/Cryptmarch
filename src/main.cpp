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


int main()
{
    //constant definitions for calculations
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int tilePixelSize = 32;

    //create the window
    GamesEngineeringBase::Window window;
    //set height and width and give it a name
    window.create(windowWidth, windowHeight, "Gauntlet");

    //create a timer
    GamesEngineeringBase::Timer timer;

    //create the asset and entity managers
    AssetManager assetManager;
    EntityManager entityManager;

    //create the player
    Player* player = new Player(100, .5f, 200, 400, 400, &assetManager.playerImage, 0, 0);
    //add the player to the entity list
    entityManager.addPlayer(player);

    //create enemies (will be created by spawners soon)
              //health, speed x y image, offsets
    //add enemies to entity list
    entityManager.addEnemy(new Enemy(100, 20, .25f, 50, 100, 200,  &assetManager.enemyImage, 0, 2, player));
    entityManager.addEnemy(new Enemy(100, 20, .25f, 100, 600, 500, &assetManager.enemyImage, 1, 2, player));
    entityManager.addEnemy(new Enemy(100, 20, .25f, 150, 300, 100, &assetManager.enemyImage, 1, 4, player));
    entityManager.addEnemy(new Enemy(100, 20, .25f, 150, 150, 300, &assetManager.enemyImage, 0, 4, player));

    //init hud
    FPS fps(&assetManager.hudImage);
    Status status(&assetManager.hudImage);

    //init map
    std::string mapPath = "resources/maps/map_arena.txt";
    Map map(&assetManager.mapImage, mapPath);

    //init camera
    Camera cam;

#ifdef _DEBUG
    bool showHitboxes = false; //are hitbox outlines currently on
    bool f1WasDown = false;    //was F1 held down last frame
#endif

    //game loop
    bool running = true;

    while (running)
    {
        if (window.keyPressed(VK_ESCAPE)) break;

        //--DELTA TIME -----------------------
        float deltaTime = timer.dt(); //get time since last frame
        float trueDelta = deltaTime; //get the true delta time for fps measurement
        //cap delta time to prevent unintended behaviour during freezes
        if (deltaTime > 0.05f) //check for very long frames
        {
            deltaTime = 0.05f; //cap the delta time to be shorter
        } //trade-off being that the game might run in slow mo during low frames



        //clear window for next frame rendering
        window.clear();


        //------------------------------ UPDATE -------------------

        entityManager.update(window, map, deltaTime);
        entityManager.cleanup();

        //set the camera to follow the player
        cam.follow(window, map, player->getX(), player->getY(), true);


        //------------------------------ DRAW ----------------------
        map.draw(window, cam);

        entityManager.draw(window, cam);

        fps.draw(window, trueDelta); //draw fps using true delta to enforce true fps readings
        status.draw(window, player->getHealth());
         
        //display frame on screen 
        window.present();
    }
    return 0;
}