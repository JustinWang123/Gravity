#ifndef MENU_H
#define MENU_H

#include "SDLWrapper.h"
#include "Button.h"
#include <string>

using namespace std;

const int BUTTON_WIDTH = 64;
const int BUTTON_HEIGHT = 16;

const int JOIN_POS_X = 10;
const int JOIN_POS_Y = 10;
const int HOST_POS_X = 10;
const int HOST_POS_Y = JOIN_POS_Y + BUTTON_HEIGHT + 10;
const int EDIT_POS_X = 10;
const int EDIT_POS_Y = HOST_POS_Y + BUTTON_HEIGHT + 10;
const int EXIT_POS_X = 10;
const int EXIT_POS_Y = EDIT_POS_Y + BUTTON_HEIGHT + 10;


enum ChoiceType {
	CHOICE_NONE,
	CHOICE_JOIN,
	CHOICE_HOST,
	CHOICE_EDIT,
	CHOICE_EXIT
};

class Menu {
public:
	Menu();
	~Menu();

	void Update();
	void Draw();

	ChoiceType GetChoice();

private:
	Button					joinButton;
	Button					hostButton;
	Button					editButton;
	Button					exitButton;

	ChoiceType choice;
};

#endif
