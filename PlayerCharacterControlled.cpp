#include "PlayerCharacterControlled.h"
#include "Game.h"

PlayerCharacterControlled :: PlayerCharacterControlled(Uint32 setId, GameSim* setGameSim)
	: 	PlayerCharacter(setId, setGameSim) {
}

void PlayerCharacterControlled :: Update(float timeDelta) {
	ProcessUserInput();
	PlayerCharacter::Update(timeDelta);
}

// ------------------------------------------------------------------------------------------------
// This is being called with a controlState that was received from the server
// It was out old controlState that we sent to the server
// We will not apply it but will instead apply all of our controlStates between then and now.
void PlayerCharacterControlled :: SetControlState(ControlState newState) {

	/*
	for(std::deque<ControlState>::iterator it = controlStates.begin(); it != controlStates.end(); it++) {
		if( (*it).time > newState.time ) {
			PlayerCharacter::SetControlState((*it));
			PlayerCharacter::Update();
			//for(int i = 0; i < (*it).length; i++)

		}
	}

	controlStates.clear();
	*/
}
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
void PlayerCharacterControlled :: ProcessUserInput() {
	controlState.upKeyIsDown = keysDown[SDLK_w];
	controlState.downKeyIsDown = keysDown[SDLK_s];
	controlState.leftKeyIsDown = keysDown[SDLK_a];
	controlState.rightKeyIsDown = keysDown[SDLK_d];
	controlState.mouse1IsDown = buttonsDown[0];
	controlState.mouse2IsDown = buttonsDown[1];
	controlState.mouse3IsDown = buttonsDown[2];
	controlState.mouse3IsDown = buttonsDown[2];
	controlState.time = SDL_GetTicks();

	float length = sqrt( pow((float)mouseX - GAME_SCREEN_WIDTH / 2,2) + pow((float)mouseY - GAME_SCREEN_HEIGHT / 2,2));
	float headingX = ((float)mouseX - GAME_SCREEN_WIDTH / 2) / length;
	float headingY = ((float)mouseY - GAME_SCREEN_HEIGHT / 2) / length;
	controlState.headingDeg = GetAngle(0, 0, headingX, headingY);

	// Every time there is a different controlState push it onto the stack:
	//if(controlStates.front() != controlState) {
		//controlStates.back().length = SDL_GetTicks() - controlStates.back().time;
		//controlStates.push_back(controlState);
	//}
} // ----------------------------------------------------------------------------------------------





