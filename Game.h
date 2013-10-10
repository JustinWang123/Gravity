#ifndef GAME_H
#define GAME_H

#include "SDLWrapper.h"
#include "PacketProtocol.h"
#include <deque>
#include <vector>
#include "GameSim.h"

const Uint32 	GAME_SCREEN_X = 4;
const Uint32 	GAME_SCREEN_Y = 4;
const Uint32 	GAME_SCREEN_WIDTH = 576;
const Uint32 	GAME_SCREEN_HEIGHT = 592;
const Uint32 	INVALID_CHANNEL = MAX_PLAYERS;

const Uint32 	CHAT_LOG_NUM_LINES_VISIBLE = 20;
const Uint32 	CHAT_LOG_LINE_LENGTH = 30;
const Uint32 	CHAT_LOG_POS_X = 590;
const Uint32 	CHAT_LOG_POS_Y = 156;
const Uint32 	CHAT_MESSAGE_POS_X = 600;
const Uint32 	CHAT_MESSAGE_POS_Y = 156;

class Game {
public:
							Game();
	virtual					~Game();

	virtual void			Update()=0;
	virtual void			Draw()=0;

protected:
    // Input methods:
	void					InputChatMessage();
	void					InputCharToChatMessage();
    void					AddMessagetoChat(std::string msg);
    void					DrawChatLog();
	void					DrawChatMessage();

	bool					exit;

    // These data members handle typing of messages by the client:
	bool 					isTypingChatMessage;
	std::string				chatMessage;
	Uint32					chatMessageCursorPos;
	std::vector<std::string>chatLog; // The chatLog stores and displays messages received by the client:

	// Network data members:
	Uint32					port;
	IPaddress				serverIP;
	UDPsocket 				socket;
	UDPpacket**				packets;
	Uint32					nextPacketTime;

	Player					players[MAX_PLAYERS];
	GameSim*				gameSim;
};

#endif
