#include "GamesEngineeringBase.h"
#include "Player.h"
#include "Graphics.h"
#include "Utilities/FPS.h"
#include "Utilities/Logger.h"

int main()
{
    GamesEngineeringBase::Window window;
    window.create(1024, 768, "Gauntlet");

    GamesEngineeringBase::Timer timer;

    Player player;
    std::string playerTileMapPath = "resources/ShooterPack/PNG/Players/Tilemap/tilemap_packed.png";
    if (!player.load(playerTileMapPath)) {
        Logger::Error("Player Tilemap failed to load at filepath: %s", playerTileMapPath);
    }

    FPS fps;
    std::string fpsTilemapPath = "resources/ShooterPack/PNG/Interface/Tilemap/tilemap_packed.png";
    if (!fps.load(fpsTilemapPath))
    {
        Logger::Error("FPS Tilemap failed to load at filepath: %s", fpsTilemapPath);
    }

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

        fps.draw(window, deltaTime);         //draw fps
        player.draw(window);
         
        //display frame on screen 
        window.present();
    }
    return 0;
}