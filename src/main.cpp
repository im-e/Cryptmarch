#include "GamesEngineeringBase.h"

#include "Utilities/AssetManager.h"
#include "Utilities/FPS.h"
#include "Utilities/Logger.h"
#include "Utilities/Vector.h"

#include "Entities/Player.h"
#include "Entities/Enemy.h"
#include "Entities/EntityManager.h"

#include "Rendering/Graphics.h"
#include "Rendering/Camera.h"

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
    Player player(100, 200, 400, 400, &assetManager.playerImage, 0, 0);
    //add the player to the entity list
    entityManager.addPlayer(&player);

    //create enemies (will be created by spawners soon)
    Enemy enemy1(100, 50, 100, 200, &assetManager.enemyImage, 0, 2, &player);
    Enemy enemy2(100, 100, 600, 500, &assetManager.enemyImage, 1, 2, &player);
    Enemy enemy3(100, 150, 300, 100, &assetManager.enemyImage, 1, 4, &player);
    Enemy enemy4(100, 200, 150, 300, &assetManager.enemyImage, 0, 4, &player);
   

    //add enemies to entity list
    entityManager.addEnemy(&enemy1);
    entityManager.addEnemy(&enemy2);
    entityManager.addEnemy(&enemy3);
    entityManager.addEnemy(&enemy4);

    //init fps
    FPS fps(&assetManager.hudImage);

    //init map
    std::string mapPath = "resources/maps/map_arena.txt";
    Map map(&assetManager.mapImage, mapPath);

    //init camera
    Camera cam;

    //game loop
    bool running = true;

    while (running)
    {
        //window.checkInput();

        if (window.keyPressed(VK_ESCAPE)) break;

        float deltaTime = timer.dt(); // once per frame only

        //clear window for next frame rendering
        window.clear();


        //------------------------------ UPDATE -------------------

        entityManager.update(window, map, deltaTime);

        //set the camera to follow the player
        cam.follow(window, map, player.getX(), player.getY(), true);

        //------------------------------ DRAW ----------------------
        map.draw(window, cam);

        entityManager.draw(window, cam);

        fps.draw(window, deltaTime); //draw fps
         
        //display frame on screen 
        window.present();
    }
    return 0;
}