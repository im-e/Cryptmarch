#include "GamesEngineeringBase.h"
#include "Player.h"
#include "Graphics.h"
#include "Utilities/FPS.h"
#include "Utilities/Logger.h"
#include "Utilities/Vector.h"
#include "Map.h"

int main()
{
    GamesEngineeringBase::Window window;
    window.create(1024, 768, "Gauntlet");

    GamesEngineeringBase::Timer timer;

    Player player;
    std::string playerTilemapPath = "resources/32rogues/rogues.png";
    if (!player.load(playerTilemapPath)) {
        Logger::Error("Player Tilemap failed to load at filepath: " + playerTilemapPath);
    }

    FPS fps;
    std::string fpsTilemapPath = "resources/ShooterPack/PNG/Interface/Tilemap/tilemap_packed.png";
    if (!fps.load(fpsTilemapPath))
    {
        Logger::Error("FPS Tilemap failed to load at filepath: " + fpsTilemapPath);
    }

    std::string mapTilemapPath = "resources/32rogues/tiles.png";
    std::string mapPath = "resources/maps/map1.txt";
    Map map(mapTilemapPath, mapPath);

    bool running = true;

    while (running)
    {
        //window.checkInput();

        if (window.keyPressed(VK_ESCAPE)) break;

        float deltaTime = timer.dt(); // once per frame only

        //clear window for next frame rendering
        window.clear();


        //update game logic
		player.update(window, deltaTime);




        //draw
        map.draw(window);

        player.draw(window);

        fps.draw(window, deltaTime); //draw fps
         
        //display frame on screen 
        window.present();
    }
    return 0;
}