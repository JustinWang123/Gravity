#include "Button.h"




// ------------------------------------------------------------------------------------------------
Button :: Button()
	:	posX(0),
		posY(0),
		isClicked(false),
		state(BUTTON_STATE_UP),
		upSurface(0),
		downSurface(0),
		overSurface(0) {
} // ----------------------------------------------------------------------------------------------


Button :: ~Button() {
	FreeSurface(upSurface);
	FreeSurface(downSurface);
	FreeSurface(overSurface);
}


void Button :: Init(int setPosX, int setPosY, SDL_Surface* setUpSurface, SDL_Surface* setOverSurface, SDL_Surface* setDownSurface) {
	posX = setPosX;
	posY = setPosY;

	upSurface = setUpSurface;
	downSurface = setDownSurface;
	overSurface = setOverSurface;
}

void Button :: Update() {
	if((Sint32)mouseX > posX
	&& (Sint32)mouseY > posY
	&& (Sint32)mouseX < posX + upSurface->w
	&& (Sint32)mouseY < posY + upSurface->h) {
		if(buttonsClicked[0]) {
			if(state == BUTTON_STATE_OVER) {
				isClicked = true;
			}

			state = BUTTON_STATE_DOWN;
		}
		else {
			state = BUTTON_STATE_OVER;
		}
	}
	else {
		state = BUTTON_STATE_UP;
	}
}

void Button :: Draw() {
	if(state == BUTTON_STATE_UP) {
		DrawSurface(posX, posY, upSurface);
	}
	else if(state == BUTTON_STATE_DOWN) {
		DrawSurface(posX, posY, downSurface);
	}
	else if(state == BUTTON_STATE_OVER) {
		DrawSurface(posX, posY, overSurface);
	}
}

bool Button :: IsUp() {
	return state == BUTTON_STATE_UP;
}

bool Button :: IsOver() {
	return state == BUTTON_STATE_OVER;
}

bool Button :: IsDown() {
	return state == BUTTON_STATE_DOWN;
}

bool Button :: IsClicked() {
	if(isClicked) {
		isClicked = false;
		return true;
	}
	else {
		return false;
	}
}
