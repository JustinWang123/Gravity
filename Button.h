#ifndef BUTTON_H
#define BUTTON_H

#include "SDLWrapper.h"

enum ButtonState
{
    BUTTON_STATE_UP,
    BUTTON_STATE_OVER,
    BUTTON_STATE_DOWN
};


class Button
{
public:
    Button();
    ~Button();

    void Init(int setPosX, int setPosY, SDL_Surface* setUpSurface, SDL_Surface* setOverSurface, SDL_Surface* setDownSurface);
    void Update();
    void Draw();

    bool IsUp();
    bool IsOver();
    bool IsDown();
    bool IsClicked();

private:
    int posX;
    int posY;

    bool isClicked;
    ButtonState state;

    SDL_Surface* upSurface;
    SDL_Surface* downSurface;
    SDL_Surface* overSurface;
};

#endif
