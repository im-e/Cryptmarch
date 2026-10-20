#include "GamesEngineeringBase.h"
#include "Player.h"
#include "Graphics.h"
#include "Utilities/FPS.h"
#include "Utilities/Logger.h"
#include "Utilities/Vector.h"
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
    Player player;
    std::string playerTilemapPath = "resources/32rogues/rogues.png"; //player tilemap path
    if (!player.load(playerTilemapPath)) { //if load returns false
        Logger::Error("Player Tilemap failed to load at filepath: " + playerTilemapPath); //assert
    }

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

    //get the world size in pixels by multiplying the map tilemap by pixel size
    float pixelWorldWidth = map.getTilemapWidth() * tilePixelSize;
    float pixelWorldHeight = map.getTilemapHeight() * tilePixelSize;

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
		player.update(window, deltaTime, pixelWorldWidth, pixelWorldHeight);

        //set the camera to follow the player
        cam.follow(player.getX(), player.getY(), windowWidth, windowHeight, true, pixelWorldWidth, pixelWorldHeight);

        //draw
        map.draw(window, cam);

        player.draw(window, cam);

        fps.draw(window, deltaTime); //draw fps
         
        //display frame on screen 
        window.present();
    }
    return 0;
}