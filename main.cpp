#include "GameServer.h"
#include "GameClient.h"
#include "GameEditor.h"
#include "SDLWrapper.h"
#include <iostream>
#include "Menu.h"

const int FRAMES_PER_SECOND = 60;
const int FRAME_TIME = 20;

enum GameMode
{
    GAME_MODE_MENU,
    GAME_MODE_GameClient,
    GAME_MODE_SERVER,
    GAME_MODE_EDITOR
};

GameMode 	gameMode;
Menu* 		menu;
GameServer* server;
GameClient* gameClient;
GameEditor*	gameEditor;

void		InitGameModeGameClient();
void		InitGameModeGameServer();
void		InitGameModeEditor();
void		InitGameModeMenu();
void		CleanUp();

int main(int argc, char *argv[])
{
    InitSDL(10 + BUTTON_WIDTH + 10, EXIT_POS_Y + BUTTON_HEIGHT + 10, "Gravity");
    PickUp::Init();

    menu = new Menu();

    int startTime = SDL_GetTicks();
    int frameTime;

    while(!quit)
    {
        startTime = SDL_GetTicks();

        HandleInput();

        if(gameMode == GAME_MODE_SERVER)
        {
            server->Update();
            server->Draw();
        }
        else if(gameMode == GAME_MODE_GameClient)
        {

            gameClient->Update();
            gameClient->Draw();

            if(gameClient->GetExit())
            {
                InitGameModeMenu();
            }
        }
        else if(gameMode == GAME_MODE_EDITOR)
        {
            gameEditor->Update();
            gameEditor->Draw();

            if(gameEditor->GetExit())
            {
                InitGameModeMenu();
            }
        }
        else if(gameMode == GAME_MODE_MENU)
        {
            menu->Update();
            menu->Draw();

            if(menu->GetChoice() == CHOICE_EXIT)
            {
                exit(0);
            }
            else if(menu->GetChoice() == CHOICE_JOIN)
            {
                InitGameModeGameClient();
            }
            else if(menu->GetChoice() == CHOICE_HOST)
            {
                InitGameModeGameServer();
            }
            else if(menu->GetChoice() == CHOICE_EDIT)
            {
                InitGameModeEditor();
            }
        }

        RenderScreen();

        frameTime = SDL_GetTicks() - startTime;
        if(frameTime < FRAME_TIME)
        {
            SDL_Delay(FRAME_TIME - frameTime);
        }
    }


    CleanUp();
    PickUp::Destroy();
    CloseSDL();

    return 0;
}

void InitGameModeMenu()
{
    CleanUp();
    gameMode = GAME_MODE_MENU;

    menu = new Menu();

    SetWindowSize(10 + BUTTON_WIDTH + 10, EXIT_POS_Y + BUTTON_HEIGHT + 10);
}

void InitGameModeGameClient()
{
    CleanUp();
    gameMode = GAME_MODE_GameClient;

    gameClient = new GameClient();

    SetWindowSize(800, 600);
}

void InitGameModeGameServer()
{
    CleanUp();
    gameMode = GAME_MODE_SERVER;

    server = new GameServer();

    SetWindowSize(800, 600);
}

void InitGameModeEditor()
{
    CleanUp();
    gameMode = GAME_MODE_EDITOR;

    gameEditor = new GameEditor();

    SetWindowSize(800, 600);
}

void CleanUp()
{
    if(gameClient != 0)
    {
        delete gameClient;
        gameClient = 0;
    }

    if(server != 0)
    {
        delete server;
        server = 0;
    }

    if(gameEditor != 0)
    {
        delete gameEditor;
        gameEditor = 0;
    }

    if(menu != 0)
    {
        delete menu;
        menu = 0;
    }
}
