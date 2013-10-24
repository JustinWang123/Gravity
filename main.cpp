#include "Server.h"
#include "Client.h"
#include "Editor.h"
#include "SDLWrapper.h"
#include <iostream>
#include "Menu.h"

const int FRAMES_PER_SECOND = 60;
const int FRAME_TIME = 20;

enum GameMode
{
    GAME_MODE_MENU,
    GAME_MODE_CLIENT,
    GAME_MODE_SERVER,
    GAME_MODE_EDITOR
};

GameMode 	gameMode;
Menu* 		menu;
Server* 	server;
Client* 	client;
Editor*		editor;

void		InitGameModeClient();
void		InitGameModeServer();
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
        else if(gameMode == GAME_MODE_CLIENT)
        {

            client->Update();
            client->Draw();

            if(client->GetExit())
            {
                InitGameModeMenu();
            }
        }
        else if(gameMode == GAME_MODE_EDITOR)
        {
            editor->Update();
            editor->Draw();

            if(editor->GetExit())
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
                InitGameModeClient();
            }
            else if(menu->GetChoice() == CHOICE_HOST)
            {
                InitGameModeServer();
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

void InitGameModeClient()
{
    CleanUp();
    gameMode = GAME_MODE_CLIENT;

    client = new Client();

    SetWindowSize(800, 600);
}

void InitGameModeServer()
{
    CleanUp();
    gameMode = GAME_MODE_SERVER;

    server = new Server();
    //client = new Client();

    SetWindowSize(800, 600);
}

void InitGameModeEditor()
{
    CleanUp();
    gameMode = GAME_MODE_EDITOR;

    editor = new Editor();

    SetWindowSize(800, 600);
}

void CleanUp()
{
    if(client != 0)
    {
        delete client;
        client = 0;
    }

    if(server != 0)
    {
        delete server;
        server = 0;
    }

    if(editor != 0)
    {
        delete editor;
        editor = 0;
    }

    if(menu != 0)
    {
        delete menu;
        menu = 0;
    }
}
