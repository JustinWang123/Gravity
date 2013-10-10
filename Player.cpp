#include "Player.h"
#include "PlayerCharacter.h"

Player :: Player() {
	state = PLAYER_STATE_DISCONECTED;
	localSequenceNum = 0;
	remoteSequenceNum = 0;
	remoteSequenceNumBitField = -1;
}

Player :: ~Player() {

}
