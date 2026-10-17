#include "GamesEngineeringBase.h"
#include "Player.h"
#include "Graphics.h"
#include "Utilities/FPS.h"
#include <iostream>

int main()
{
    GamesEngineeringBase::Window window;
    window.create(1024, 768, "Gauntlet");

    GamesEngineeringBase::Timer timer;

    Player player;
	player.load("resources/player.png");

    GamesEngineeringBase::Image image;
    image.load("resources/ShooterPack/PNG/Players/Tilemap/tilemap_packed.png");

    FPS fps;
    if (!fps.load("resources/ShooterPack/PNG/Interface/Tilemap/tilemap_packed.png"))
    {
        std::cout << "fps image not loaded" << std::endl;
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