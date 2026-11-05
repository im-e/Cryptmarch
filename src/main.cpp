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

    //input helpers
    bool enterWasDown = false;
    bool upWasDown = false;
    bool downWasDown = false;

    //game loop
    bool running = true;

    while (running)
    {
        //----------------- DELTA TIME -----------------------
        float deltaTime = timer.dt(); //get time since last frame
        float trueDelta = deltaTime; //get the true delta time for fps measurement
        //cap delta time to prevent unintended behaviour during freezes
        if (deltaTime > 0.05f) //check for very long frames
        {
            deltaTime = 0.05f; //cap the delta time to be shorter
        } //trade-off being that the game might run in slow mo during low frames


        //true only on the frame enter/up/down goes down
        bool enterDown = window.keyPressed(VK_RETURN);
        bool enterPressed = enterDown && !enterWasDown;
        enterWasDown = enterDown;

        bool upDown = window.keyPressed(VK_UP);
        bool upPressed = upDown && !upWasDown;
        upWasDown = upDown;

        bool downDown = window.keyPressed(VK_DOWN);
        bool downPressed = downDown && !downWasDown;
        downWasDown = downDown;

        //clear window for next frame rendering
        window.clear();


        switch (gameController.getState())
        {
        case MenuState: {
            Graphics::DrawImage(window, assetManager.mainMenuImage, 0, 0); //draw main menu image

            const char* title = "CRYPTMARCH"; //title 
            const char* prompt = "PRESS ENTER TO START"; //prompt
            int titleX = (windowWidth - (int)strlen(title) * 32) / 2; //use strlen to find the length of the string for centering
            int promptX = (windowWidth - (int)strlen(prompt) * 32) / 2;
            Graphics::DrawString(window, assetManager.hudImage, title, titleX, 200); //draw to screen
            Graphics::DrawString(window, assetManager.hudImage, prompt, promptX, 500);

            if (enterPressed) //if enter pressed
            {
                gameController.setState(PlayerSelectState); //go to player select state
            }
            break;
        }
        case PlayerSelectState: {
            Graphics::DrawImage(window, assetManager.mainMenuImage, 0, 0); //draw main menu image

            const char* title = "PLAYER SELECT"; //title
            int titleX = (windowWidth - (int)strlen(title) * 32) / 2; //centre
            Graphics::DrawString(window, assetManager.hudImage, title, titleX, 180); //draw


            //get the index and change based on input
            int index = gameController.getPlayerSelectIndex(); //get the index of current selection
            if (upPressed) //if up is pressed
            { //ternary operator ? - if true - result 1 , if false result 2
                index = (index == 0) ? 1 : 0;  //set index to - (if index == 0) set to 1 if true, 0 if false
            }
            if (downPressed) //if down is pressed
            { 
                index = (index == 1) ? 0 : 1; 
            }
            //set the index
            gameController.setPlayerSelectIndex(index); //set player selection index

            //depending on the index value, change the options to highlight which index is current to player selection
            const char* option1 = (index == 0) ? "- SINGLE PLAYER -" : "  SINGLE PLAYER  ";
            const char* option2 = (index == 1) ? "- MULTIPLAYER   -" : "  MULTIPLAYER    ";

            //draw both
            Graphics::DrawString(window, assetManager.hudImage, option1, (windowWidth - (int)strlen(option1) * 32) / 2, 300);
            Graphics::DrawString(window, assetManager.hudImage, option2, (windowWidth - (int)strlen(option2) * 32) / 2, 380);

            if (enterPressed) //if enter pressed
            {
                //confirm selection
                gameController.confirmPlayerSelection();
            }
            break;
        }
        case CharacterSelectState: {
            Graphics::DrawImage(window, assetManager.mainMenuImage, 0, 0); //draw background image

            const char* title = "PLAYER 1: CHOOSE HERO"; //title
            int titleX = (windowWidth - (int)strlen(title) * 32) / 2; //centre title
            Graphics::DrawString(window, assetManager.hudImage, title, titleX, 180); //draw title

            int index = gameController.getCharacterSelectIndex(); //get index

            //menu nav wrap around
            if (upPressed) 
            {   //%3 will always give back 0,1,2 
                //we add + 3 to always do a full loop around to stop index from being -1 which would break the wrap around
                index = (index - 1 + 3) % 3; 
            }
            if (downPressed) 
            { 
                //if index is 2, + 1 = 3 , %3 = 0, wrapping it back around
                index = (index + 1) % 3; 
            }
            //set the index 
            gameController.setCharacterSelectIndex(index);

            //using the index we will choose which highlight is shown
            const char* option1 = (index == 0) ? "- DWARF  -" : "  DWARF  ";
            const char* option2 = (index == 1) ? "- ARCHER -" : "  ARCHER ";
            const char* option3 = (index == 2) ? "- WIZARD -" : "  WIZARD ";

            //draw the options with centering
            Graphics::DrawString(window, assetManager.hudImage, option1, (windowWidth - (int)strlen(option1) * 32) / 2, 260);
            Graphics::DrawString(window, assetManager.hudImage, option2, (windowWidth - (int)strlen(option2) * 32) / 2, 330);
            Graphics::DrawString(window, assetManager.hudImage, option3, (windowWidth - (int)strlen(option3) * 32) / 2, 400);

            //if enter pressed
            if (enterPressed)
            {
                //confirm selection
                gameController.confirmCharacterSelection();
            }
            break;
        }
        case P2CharacterSelectState: {

            Graphics::DrawImage(window, assetManager.mainMenuImage, 0, 0); //draw background image

            const char* title = "PLAYER 2: CHOOSE HERO"; //title
            int titleX = (windowWidth - (int)strlen(title) * 32) / 2; //centre title
            Graphics::DrawString(window, assetManager.hudImage, title, titleX, 180); //draw title

            int index = gameController.getP2CharacterSelectIndex(); //get index
            //menu nav wrap around
            if (upPressed)
            {   //%3 will always give back 0,1,2 
                //we add + 3 to always do a full loop around to stop index from being -1 which would break the wrap around
                index = (index - 1 + 3) % 3;
            }
            if (downPressed)
            {
                //if index is 2, + 1 = 3 , %3 = 0, wrapping it back around
                index = (index + 1) % 3;
            }

            //set the index 
            gameController.setP2CharacterSelectIndex(index);

            //using the index we will choose which highlight is shown
            const char* opt1 = (index == 0) ? "- DWARF  -" : "  DWARF  ";
            const char* opt2 = (index == 1) ? "- ARCHER -" : "  ARCHER ";
            const char* opt3 = (index == 2) ? "- WIZARD -" : "  WIZARD ";

            //draw the options with centering
            Graphics::DrawString(window, assetManager.hudImage, opt1, (windowWidth - (int)strlen(opt1) * 32) / 2, 260);
            Graphics::DrawString(window, assetManager.hudImage, opt2, (windowWidth - (int)strlen(opt2) * 32) / 2, 330);
            Graphics::DrawString(window, assetManager.hudImage, opt3, (windowWidth - (int)strlen(opt3) * 32) / 2, 400);

            //if enter pressed
            if (enterPressed)
            {
                //confirm selection
                gameController.confirmP2CharacterSelection();
            }
            break;
        }
        case ModeSelectState: {
            //draw background image
            Graphics::DrawImage(window, assetManager.mainMenuImage, 0, 0);

            const char* title = "SELECT MODE"; //set title
            int titleX = (windowWidth - (int)strlen(title) * 32) / 2; //centre title
            Graphics::DrawString(window, assetManager.hudImage, title, titleX, 180); //draw title

            int index = gameController.getModeSelectIndex(); //get index
            if (upPressed) 
            {   //1 if true, 0 if false
                index = (index == 0) ? 1 : 0; 
            }
            if (downPressed) 
            {   //reverse
                index = (index == 1) ? 0 : 1; 
            }
            //set index
            gameController.setModeSelectIndex(index);

            //based on index show the highlight
            const char* opt1 = (index == 0) ? "- STORY MODE    -" : "  STORY MODE     ";
            const char* opt2 = (index == 1) ? "- INFINITE MODE -" : "  INFINITE MODE  ";

            //draw image
            Graphics::DrawString(window, assetManager.hudImage, opt1, (windowWidth - (int)strlen(opt1) * 32) / 2, 300);
            Graphics::DrawString(window, assetManager.hudImage, opt2, (windowWidth - (int)strlen(opt2) * 32) / 2, 380);

            //if enter pressed
            if (enterPressed)
            {
                //start new game
                gameController.startNewGame(assetManager, entityManager, map);
            }
            break;
        }
            case PlayState: { //GAME STATE
                //------------------------------ UPDATE -------------------
                entityManager.update(window, map, deltaTime); //update all entities
                gameController.updateInfiniteSpawners(assetManager, entityManager, map); //updates infinite level spawners
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


                //camera control
                Player* p1 = entityManager.getPlayerById(0); //get both players
                Player* p2 = entityManager.getPlayerById(1);

                if (p1 != nullptr && p2 != nullptr) //if both arent null
                {
                    float midpointX = (p1->getX() + p2->getX()) / 2; //get the midpoint of both players
                    float midpointY = (p1->getY() + p2->getY()) / 2; //get the midpoint of both players

                    cam.follow(window, map, midpointX, midpointY, !map.isInfinite());
                }
                else if (p1 != nullptr) //otherwise prioritise p1 camera
                {
                    cam.follow(window, map, p1->getX(), p1->getY(), !map.isInfinite()); 
                }
                else if (p2 != nullptr) //otherwise p1 is dead, so follow p2 instead
                {
                    cam.follow(window, map, p2->getX(), p2->getY(), !map.isInfinite()); 
                }

                   
         

                //increase stats
                gameController.getStats().timeSurvived += deltaTime;

                //------------------------------ DRAW ----------------------
               
                map.draw(window, cam); //draw map

                entityManager.draw(window, cam); //draw entities

                fps.draw(window, trueDelta); //draw fps using true delta to enforce true fps readings

                //draw status for players
                for (int i = 0; i < entityManager.getPlayerCount(); i++)
                {
                    status.draw(window, entityManager.getPlayer(i));
                }

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