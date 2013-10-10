#ifndef CLIENT_H
#define CLIENT_H

#include "Game.h"
#include <deque>
#include <vector>
#include "Button.h"
class PlayerCharacter;

const Uint32 	GAME_EXIT_BUTTON_POS_X = 666;
const Uint32 	GAME_EXIT_BUTTON_POS_Y = 570;
const Uint32 	SERVER = 0;
const Uint32 	SERVER_INACTIVITY_TIME = 5000; // 5 seconds with no messages means inactive
const Uint32	RECONNECT_TO_SERVER_DELAY = 5000; // 5 seconds
const Uint32	MIN_SEND_CONTROL_STATE_TIME = 30;

class Client : public Game {
public:
							Client();
							~Client();

	virtual void			Update();
	virtual void			Draw();

	bool					GetExit() const {return exit;}

private:
	void					UpdateServerConnection();
	void					UpdateCamera();

	// Character Control State:
	void					HandleMyPcControlState();

	// Drawing methods:
	void					DrawHUD();
	void					DrawScoreBoard();

	// Network methods:
	void					SendDataAsPacket(char data[], int length);
    Uint32          		GetLostPacketSequenceNum(Uint32 ackNum, Uint32 ackBitfield);
	void					AckSequenceNum(Uint32 num);
	void					StorePacket(char data[], Uint32 length);
	ListablePacket*			GetStoredPacket(Uint32 sequenceNum);

	// High level network receivers:
	void 					ReceiveNetworkData();

	// Handling of event packets:
	void					HandleTextMessage(UDPpacket* packet);
	void					HandleAddNewClient(UDPpacket* packet);
	void					HandleRemoveClient(UDPpacket* packet);
	void					HandleConnectionAccepted(UDPpacket* packet);
	void					HandleSpawn(UDPpacket* packet);
	void					HandleKillNotification(UDPpacket* packet);
	void					HandleUpdateWorld(UDPpacket* packet);

	// Sending event packets:
	void					SendUpdateServer();
	void					SendTextMessage(std::string msg);
	void 					SendConnectionRequest(std::string name);
	void					SendDisconnectNotification();
	void					SendControlState(ControlState state);

	// data members:
	PlayerCharacter*		myPc;
	Uint32					myChannel;
	std::string 			clientName;
	bool					isMouseOverExitButton;
	Uint32					serverPingTime;
	Uint32					timeOfLastServerMessage;
	Uint32					timeOfLastMessageToServer;
	Uint32					timeToSendNextControlState;
	bool					isConnectedToServer;


	std::deque<ControlState> previousControlStates;
	ControlState			lastControlState;
	Uint32					lastMouseX;
	Uint32					lastMouseY;
	Uint32					lastHeadingDeg;
	Uint32					timeOfLastHeadingMessage;

	// Interface:
	Button					exitButton;
	SDL_Surface*			inGameMenu;

	// Network stuff:
	Uint32 					localSequenceNum;
	Uint32					remoteSequenceNum;
	Uint32					remoteSequenceNumBitField;
	std::deque<ListablePacket*> sentPacketList;
	Vector2df				camPos;
};
#endif
