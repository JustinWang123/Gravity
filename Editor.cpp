#include "Editor.h"
#include "EditorGame.h"
#include "EditorMap.h"

const Uint32 	GAME_SAVE_BUTTON_POS_X = 666;
const Uint32 	GAME_SAVE_BUTTON_POS_Y = 528;
const Uint32 	GAME_EXIT_BUTTON_POS_X = 666;
const Uint32 	GAME_EXIT_BUTTON_POS_Y = 570;

Editor :: Editor()
    : Game()
{
    camPos.Set(0,0);

    inGameMenu = LoadSurface("Surfaces/InGameMenu.png");

    exitButton.Init(GAME_EXIT_BUTTON_POS_X, GAME_EXIT_BUTTON_POS_Y,
                    LoadSurface("Surfaces/ExitButtonUp.png"),
                    LoadSurface("Surfaces/ExitButtonOver.png"),
                    LoadSurface("Surfaces/ExitButtonUp.png"));

    saveButton.Init(GAME_SAVE_BUTTON_POS_X, GAME_SAVE_BUTTON_POS_Y,
                    LoadSurface("Surfaces/SaveButtonUp.png"),
                    LoadSurface("Surfaces/SaveButtonOver.png"),
                    LoadSurface("Surfaces/SaveButtonUp.png"));

    exit = false;
    game = new EditorGame();
    editorMap = ((EditorGame*)game)->GetMap();
    currentTile = 0;
    selector = LoadSurface("Surfaces/EditorSelector.png");
}


Editor :: ~Editor()
{
    FreeSurface(selector);
}

void Editor :: Update()
{
    if(keysDown[SDLK_w])
    {
        camPos.y -= EDITOR_CAM_SPEED;
    }
    else if(keysDown[SDLK_s])
    {
        camPos.y += EDITOR_CAM_SPEED;
    }

    if(keysDown[SDLK_a])
    {
        camPos.x -= EDITOR_CAM_SPEED;
    }
    else if(keysDown[SDLK_d])
    {
        camPos.x += EDITOR_CAM_SPEED;
    }

    exitButton.Update();
    saveButton.Update();

    if(mouseX > 768 && buttonsClicked[0])
    {
        currentTile = mouseY / 32;
    }

    if(mouseX < 580 && buttonsDown[0])
    {
        editorMap->SetTile( (camPos.x + mouseX)/32, (camPos.y + mouseY)/32, (TileType)currentTile);
    }

    // Exit:
    if(exitButton.IsClicked())
    {
        exit = true;
    }

    // Save Level:
    if(saveButton.IsClicked())
    {
        editorMap->SaveMap();
    }
}

void Editor :: Draw()
{
    game->Draw(camPos);
    DrawSurface(580, 0, inGameMenu);
    exitButton.Draw();
    saveButton.Draw();

    for(int i = 0; i < NUM_OF_TILE_TYPES; i++)
    {
        DrawSurface(768, i * 32, editorMap->GetTileSurface((TileType)i));
    }

    DrawSurface(768, currentTile * 32, selector);
}

void Editor :: DrawTileSelector()
{
}
