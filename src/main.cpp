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
            Graphics::DrawImage(window, assetManager.mainMenuImage, 0, 0);

            const char* title = "CRYPTMARCH";
            const char* prompt = "PRESS ENTER TO START";
            int titleX = (windowWidth - (int)strlen(title) * 32) / 2;
            int promptX = (windowWidth - (int)strlen(prompt) * 32) / 2;
            Graphics::DrawString(window, assetManager.hudImage, title, titleX, 200);
            Graphics::DrawString(window, assetManager.hudImage, prompt, promptX, 500);

            if (enterPressed)
            {
                gameController.setState(PlayerSelectState);
            }
            break;
        }
        case PlayerSelectState: {
            Graphics::DrawImage(window, assetManager.mainMenuImage, 0, 0);

            const char* title = "PLAYER SELECT";
            int titleX = (windowWidth - (int)strlen(title) * 32) / 2;
            Graphics::DrawString(window, assetManager.hudImage, title, titleX, 180);

            int idx = gameController.getPlayerSelectIndex();
            if (upPressed) { idx = (idx == 0) ? 1 : 0; }
            if (downPressed) { idx = (idx == 1) ? 0 : 1; }
            gameController.setPlayerSelectIndex(idx);

            const char* opt1 = (idx == 0) ? "- SINGLE PLAYER -" : "  SINGLE PLAYER  ";
            const char* opt2 = (idx == 1) ? "- MULTIPLAYER   -" : "  MULTIPLAYER    ";

            Graphics::DrawString(window, assetManager.hudImage, opt1, (windowWidth - (int)strlen(opt1) * 32) / 2, 300);
            Graphics::DrawString(window, assetManager.hudImage, opt2, (windowWidth - (int)strlen(opt2) * 32) / 2, 380);

            if (enterPressed)
            {
                gameController.confirmPlayerSelection();
            }
            break;
        }
        case CharacterSelectState: {
            Graphics::DrawImage(window, assetManager.mainMenuImage, 0, 0);

            const char* title = "PLAYER 1: CHOOSE HERO";
            int titleX = (windowWidth - (int)strlen(title) * 32) / 2;
            Graphics::DrawString(window, assetManager.hudImage, title, titleX, 180);

            int idx = gameController.getCharacterSelectIndex();
            if (upPressed) { idx = (idx - 1 + 3) % 3; }
            if (downPressed) { idx = (idx + 1) % 3; }
            gameController.setCharacterSelectIndex(idx);

            const char* opt1 = (idx == 0) ? "- DWARF  -" : "  DWARF  ";
            const char* opt2 = (idx == 1) ? "- ARCHER -" : "  ARCHER ";
            const char* opt3 = (idx == 2) ? "- WIZARD -" : "  WIZARD ";

            Graphics::DrawString(window, assetManager.hudImage, opt1, (windowWidth - (int)strlen(opt1) * 32) / 2, 260);
            Graphics::DrawString(window, assetManager.hudImage, opt2, (windowWidth - (int)strlen(opt2) * 32) / 2, 330);
            Graphics::DrawString(window, assetManager.hudImage, opt3, (windowWidth - (int)strlen(opt3) * 32) / 2, 400);

            if (enterPressed)
            {
                gameController.confirmCharacterSelection();
            }
            break;
        }
        case P2CharacterSelectState: {
            Graphics::DrawImage(window, assetManager.mainMenuImage, 0, 0);

            const char* title = "PLAYER 2: CHOOSE HERO";
            int titleX = (windowWidth - (int)strlen(title) * 32) / 2;
            Graphics::DrawString(window, assetManager.hudImage, title, titleX, 180);

            int idx = gameController.getP2CharacterSelectIndex();
            if (upPressed) { idx = (idx - 1 + 3) % 3; }
            if (downPressed) { idx = (idx + 1) % 3; }
            gameController.setP2CharacterSelectIndex(idx);

            const char* opt1 = (idx == 0) ? "- DWARF  -" : "  DWARF  ";
            const char* opt2 = (idx == 1) ? "- ARCHER -" : "  ARCHER ";
            const char* opt3 = (idx == 2) ? "- WIZARD -" : "  WIZARD ";

            Graphics::DrawString(window, assetManager.hudImage, opt1, (windowWidth - (int)strlen(opt1) * 32) / 2, 260);
            Graphics::DrawString(window, assetManager.hudImage, opt2, (windowWidth - (int)strlen(opt2) * 32) / 2, 330);
            Graphics::DrawString(window, assetManager.hudImage, opt3, (windowWidth - (int)strlen(opt3) * 32) / 2, 400);

            if (enterPressed)
            {
                gameController.confirmP2CharacterSelection();
            }
            break;
        }
        case ModeSelectState: {
            Graphics::DrawImage(window, assetManager.mainMenuImage, 0, 0);

            const char* title = "SELECT MODE";
            int titleX = (windowWidth - (int)strlen(title) * 32) / 2;
            Graphics::DrawString(window, assetManager.hudImage, title, titleX, 180);

            int idx = gameController.getModeSelectIndex();
            if (upPressed) { idx = (idx == 0) ? 1 : 0; }
            if (downPressed) { idx = (idx == 1) ? 0 : 1; }
            gameController.setModeSelectIndex(idx);

            const char* opt1 = (idx == 0) ? "- STORY MODE    -" : "  STORY MODE     ";
            const char* opt2 = (idx == 1) ? "- INFINITE MODE -" : "  INFINITE MODE  ";

            Graphics::DrawString(window, assetManager.hudImage, opt1, (windowWidth - (int)strlen(opt1) * 32) / 2, 300);
            Graphics::DrawString(window, assetManager.hudImage, opt2, (windowWidth - (int)strlen(opt2) * 32) / 2, 380);

            if (enterPressed)
            {
                gameController.startNewGame(assetManager, entityManager, map);
            }
            break;
        }
            case PlayState: { //GAME STATE
                //------------------------------ UPDATE -------------------
                entityManager.update(window, map, deltaTime); //update all entities
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

                //set the camera to follow the player
                Player* p1 = entityManager.getFirstPlayer();
                cam.follow(window, map, p1 ->getX(), p1->getY(), true);

                //increase stats
                gameController.getStats().timeSurvived += deltaTime;

                //------------------------------ DRAW ----------------------
                map.draw(window, cam);

                entityManager.draw(window, cam);
                fps.draw(window, trueDelta); //draw fps using true delta to enforce true fps readings

                Player* player = entityManager.getFirstPlayer();
                status.draw(window, player->getHealth(), player->getMaxHealth(), player->getMana(), player->getMaxMana());

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