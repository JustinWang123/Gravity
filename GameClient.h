#ifndef GameClient_H
#define GameClient_H

#include "GameBase.h"
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

class GameClient : public GameBase
{
public:
    GameClient();
    ~GameClient();

    virtual void			Update();
    virtual void			Draw();

     // Events:
    virtual void			DamageCharsInCircle(Uint32 attackerID, Uint32 damage, Vector2df centerPos, float radius) {}
    virtual void			DamageCharsAtPos(Uint32 attackerID, Uint32 damage, Vector2df atPos) {}

    // Creates:
    virtual void			SpawnPlayerCharacter(Uint32 playerID) {}
    virtual void			CreateProjectile(ProjectileType type, int ownerPlayerID, Vector2df setPos, Vector2df setHeading) {}
    virtual void			CreateExplosion(int ownerPlayerID, Vector2df setPos) {}
    virtual void			CreatePickUp(Vector2df setPos, PickUpType setType) {}

private:
    void					UpdateGameServerConnection();
    void					UpdateCamera();
	virtual void			UpdateRespawn(float timeDelta) {}

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
    void					HandleAddNewGameClient(UDPpacket* packet);
    void					HandleRemoveGameClient(UDPpacket* packet);
    void					HandleConnectionAccepted(UDPpacket* packet);
    void					HandleSpawn(UDPpacket* packet);
    void					HandleKillNotification(UDPpacket* packet);
    void					HandleUpdateWorld(UDPpacket* packet);

    // Sending event packets:
    void					SendUpdateGameServer();
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
    Uint32					timeOfLastGameServerMessage;
    Uint32					timeOfLastMessageToGameServer;
    Uint32					timeToSendNextControlState;
    bool					isConnectedToGameServer;


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
