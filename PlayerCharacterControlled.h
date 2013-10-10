#ifndef PLAYER_CHARACTER_CONTROLLED_H
#define PLAYER_CHARACTER_CONTROLLED_H

#include "PlayerCharacter.h"
#include <deque>

class PlayerCharacterControlled : public PlayerCharacter {
public:
							PlayerCharacterControlled(Uint32 setId, GameSim* setGameSim);

	virtual void			Update(float timeDelta);

private:
	virtual void			SetControlState(ControlState newState);
	void					ProcessUserInput();

	std::deque<ControlState> controlStates;
};
#endif // PLAYER_CHARACTER_CONTROLLED_H
