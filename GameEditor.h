#ifndef EDITOR_GAME_SIM_H
#define EDITOR_GAME_SIM_H

#include "GameBase.h"
#include "Button.h"
const float EDITOR_CAM_SPEED =  5.0f;


class GameEditor : public GameBase
{
public:
    GameEditor();
    virtual ~GameEditor();

    virtual void			Update();
    virtual void			Draw();

private:
    Button					exitButton;
    Button					saveButton;
    SDL_Surface*			inGameMenu;

    Vector2df camPos;

    int 					currentTile;
    SDL_Surface*			selector;
};
#endif // EDITOR_GAME_SIM_H
