#include "Menu.h"

Menu :: Menu()
{
    joinButton.Init(JOIN_POS_X, JOIN_POS_Y,
                    LoadSurface("Surfaces/JoinButtonUp.png"),
                    LoadSurface("Surfaces/JoinButtonOver.png"),
                    LoadSurface("Surfaces/JoinButtonUp.png"));

    hostButton.Init(HOST_POS_X, HOST_POS_Y,
                    LoadSurface("Surfaces/HostButtonUp.png"),
                    LoadSurface("Surfaces/HostButtonOver.png"),
                    LoadSurface("Surfaces/HostButtonUp.png"));

    editButton.Init(EDIT_POS_X, EDIT_POS_Y,
                    LoadSurface("Surfaces/EditButtonUp.png"),
                    LoadSurface("Surfaces/EditButtonOver.png"),
                    LoadSurface("Surfaces/EditButtonUp.png"));


    exitButton.Init(EXIT_POS_X, EXIT_POS_Y,
                    LoadSurface("Surfaces/ExitButtonUp.png"),
                    LoadSurface("Surfaces/ExitButtonOver.png"),
                    LoadSurface("Surfaces/ExitButtonUp.png"));

    choice = CHOICE_NONE;
}

Menu :: ~Menu()
{
}

void Menu :: Update()
{
    joinButton.Update();
    hostButton.Update();
    editButton.Update();
    exitButton.Update();

    if(exitButton.IsDown())
    {
        choice = CHOICE_EXIT;
    }
    else if(joinButton.IsDown())
    {
        choice = CHOICE_JOIN;
    }
    else if(hostButton.IsDown())
    {
        choice = CHOICE_HOST;
    }
    else if(editButton.IsDown())
    {
        choice = CHOICE_EDIT;
    }
}

void Menu :: Draw()
{
    joinButton.Draw();
    hostButton.Draw();
    editButton.Draw();
    exitButton.Draw();
}

ChoiceType Menu :: GetChoice()
{
    return choice;
};
