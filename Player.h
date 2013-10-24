#ifndef PLAYER_H
#define PLAYER_H

#include "PlayerCharacter.h"
#include <deque>

const Uint32    LISTABLE_PACKET_DATA_LENGTH = 100;

struct ListablePacket
{
    char data[LISTABLE_PACKET_DATA_LENGTH];
    Uint32 length;
};

enum PlayerState
{
    PLAYER_STATE_DISCONECTED,
    PLAYER_STATE_ACTIVE
};

struct Player
{
    Player();
    ~Player();

    PlayerState 			state;
    IPaddress 				ipAddress;
    std::string 			name;
    Uint32					timeOfLastMessage;

    Uint32 					localSequenceNum;
    Uint32					remoteSequenceNum;
    Uint32					remoteSequenceNumBitField;
    std::deque<ListablePacket*> sentPacketList;
};
#endif
