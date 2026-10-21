#include "GamesEngineeringBase.h"

#include "Utilities/FPS.h"
#include "Utilities/Logger.h"
#include "Utilities/Vector.h"
#include "Player.h"
#include "Graphics.h"
#include "Enemy.h"
#include "Map.h"
#include "Camera.h"

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

    //init player
    std::string playerTilemapPath = "resources/32rogues/rogues.png"; //player tilemap path
    Player player(100, 200, 400, 400, playerTilemapPath, 0, 0);

    //init enemy
    std::string enemyTilemapPath = "resources/32rogues/monsters.png";
    Vector<Enemy*> enemies;

    Enemy enemy1(100, 50, 100, 200, enemyTilemapPath, 0, 2, &player);
    Enemy enemy2(100, 100, 600, 500, enemyTilemapPath, 1, 2, &player);
    Enemy enemy3(100, 150, 300, 100, enemyTilemapPath, 1, 4, &player);
    Enemy enemy4(100, 200, 150, 300, enemyTilemapPath, 0, 4, &player);
   
    enemies.add(&enemy1);
    enemies.add(&enemy2);
    enemies.add(&enemy3);
    enemies.add(&enemy4);

   

    //init fps
    FPS fps; 
    std::string fpsTilemapPath = "resources/ShooterPack/PNG/Interface/Tilemap/tilemap_packed.png";
    if (!fps.load(fpsTilemapPath))
    {
        Logger::Error("FPS Tilemap failed to load at filepath: " + fpsTilemapPath);
    }

    //init map
    std::string mapTilemapPath = "resources/32rogues/tiles.png";
    std::string mapPath = "resources/maps/map_arena.txt";
    Map map(mapTilemapPath, mapPath);

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


        //update game logic
		player.update(window, map, deltaTime);

        for (int i = 0; i < enemies.getSize(); i++)
        {
            enemies[i]->update(window, map, deltaTime);
        }


        //set the camera to follow the player
        cam.follow(window, map, player.getX(), player.getY(), true);

        //draw
        map.draw(window, cam);

        for (int i = 0; i < enemies.getSize(); i++)
        {
            enemies[i]->draw(window, cam);
        }

        player.draw(window, cam);

        fps.draw(window, deltaTime); //draw fps
         
        //display frame on screen 
        window.present();
    }
    return 0;
}