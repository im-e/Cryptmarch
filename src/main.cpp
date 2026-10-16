#include "GamesEngineeringBase.h"
#include "Player.h"
#include "Graphics.h"

int main()
{
    GamesEngineeringBase::Window window;
    window.create(1024, 768, "Gauntlet");

    GamesEngineeringBase::Timer timer;

    Player player;
	player.load("resources/player.png");

    bool running = true;


    while (running)
    {
        //window.checkInput();

        if (window.keyPressed(VK_ESCAPE)) break;


        float deltaTime = timer.dt();          // once per frame only
       


        //cleart window for next frame rendering
        window.clear();


        //update game logic
		player.update(window, deltaTime);

        //draw
        player.draw(window);
         
        //display frame on screen 
        window.present();
    }
    return 0;
}