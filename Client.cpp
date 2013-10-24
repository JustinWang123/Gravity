#include "Client.h"
#include <math.h>
#include <assert.h>
#include <fstream>
using namespace std;




// ------------------------------------------------------------------------------------------------
Client :: Client()
{
    // Open a UDPsocket on the first unused socket:
    for (int i = 0; i < 10; i++)
    {
        socket = SDLNet_UDP_Open(NETWORK_PORT + i);

        if(socket != NULL)
        {
            port = NETWORK_PORT + i;
            break;
        }
    }

    std::ifstream clientFile ("data/ClientInit.txt");
    assert(clientFile.is_open());
    std::string str;
    std::string ipAddressStr;
    clientFile >> str;
    assert(str == "ServerIP:" && "Somethings wrong with data/ClientInit.txt");
    clientFile >> ipAddressStr;
    clientFile >> str;
    assert(str == "ClientName:" && "Somethings wrong with data/ClientInit.txt");
    clientFile >> clientName;
    clientFile.close();

    if(SDLNet_ResolveHost(&serverIP, "64.46.19.58", NETWORK_PORT))
    {
        AddMessagetoChat("Failed to resolve host");
    }

    // Bind the server to channel 0:
    if(SDLNet_UDP_Bind(socket, SERVER, &serverIP))
    {
        AddMessagetoChat("Failed to bind socket");
    }

    inGameMenu = LoadSurface("Surfaces/InGameMenu.png");

    exitButton.Init(GAME_EXIT_BUTTON_POS_X, GAME_EXIT_BUTTON_POS_Y,
                    LoadSurface("Surfaces/ExitButtonUp.png"),
                    LoadSurface("Surfaces/ExitButtonOver.png"),
                    LoadSurface("Surfaces/ExitButtonUp.png"));

    // No players are active at first
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        players[i].state = PLAYER_STATE_DISCONECTED;
    }

    isTypingChatMessage = false;
    myChannel = INVALID_CHANNEL;
    exit = false;
    myPc = 0;
    localSequenceNum = 0;
    remoteSequenceNum = 0;
    remoteSequenceNumBitField = -1;
    timeToSendNextControlState = SDL_GetTicks();
    game = new BaseGame();
    timeOfLastServerMessage = SDL_GetTicks();
    nextPacketTime = SDL_GetTicks();
} // ------------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Client :: ~Client()
{
    // Notifiy server that we're disconnecting:
    if(isConnectedToServer)
    {
        SendDisconnectNotification();
    }

    FreeSurface(inGameMenu);

    if ( socket != NULL )
    {
        SDLNet_UDP_Close(socket);
        socket = NULL;
    }
    if ( packets != NULL )
    {
        SDLNet_FreePacketV(packets);
        packets = NULL;
    }

    delete game;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: Update()
{
    // Receive all network data this game loop:
    ReceiveNetworkData();

    // Update everything:
    InputChatMessage();
    HandleMyPcControlState();
    game->Update();
    UpdateCamera();
    UpdateServerConnection();
    exitButton.Update();

    // Exit:
    if(exitButton.IsClicked())
    {
        exit = true;
    }

    // Send network data:
    if(isConnectedToServer && SDL_GetTicks() > nextPacketTime)
    {
        SendUpdateServer();
        nextPacketTime = SDL_GetTicks() + PACKET_SEND_DELAY;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: UpdateServerConnection()
{
    // Keep attempting to connect to server:
    if(!isConnectedToServer && SDL_GetTicks() > nextPacketTime)
    {
        nextPacketTime = SDL_GetTicks() + RECONNECT_TO_SERVER_DELAY;
        timeOfLastServerMessage = SDL_GetTicks();
        SendConnectionRequest(clientName);
    }

    // Determine if we have lost connection to server
    if(isConnectedToServer && SDL_GetTicks() - timeOfLastServerMessage > SERVER_INACTIVITY_TIME)
    {
        AddMessagetoChat("Server not responding...");
        isConnectedToServer = false;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: UpdateCamera()
{
    if(myPc != 0)
    {
        float desiredCamPosX = myPc->GetPos().x - GAME_SCREEN_WIDTH / 2;
        camPos.x -= (camPos.x - desiredCamPosX) / 4.0f;
        float desiredCamPosY = myPc->GetPos().y - GAME_SCREEN_HEIGHT / 2;
        camPos.y -= (camPos.y - desiredCamPosY) / 4.0f;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: HandleMyPcControlState()
{
    if(myPc != 0)
    {
        ControlState currentControlState = myPc->GetControlState();

        // Send the keys to the server if it is different then the previous one:
        if(currentControlState != lastControlState && SDL_GetTicks() > timeToSendNextControlState)
        {
            timeToSendNextControlState = SDL_GetTicks() + MIN_SEND_CONTROL_STATE_TIME;
            SendControlState(currentControlState);
            lastControlState = currentControlState;
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: Draw()
{
    game->Draw(camPos);
    DrawHUD();
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
void Client :: DrawHUD()
{
    DrawSurface(580, 0, inGameMenu);
    DrawChatLog();
    DrawChatMessage();
    DrawScoreBoard();
    exitButton.Draw();
} // ----------------------------------------------------------------------------------------------







// ------------------------------------------------------------------------------------------------
void Client :: DrawScoreBoard()
{

    if(myChannel != INVALID_CHANNEL)
    {
        DrawText(610,10, "Health: " + ToString(myPc->GetHealth()));
        DrawText(610,25, "Bullets: " + ToString(myPc->GetNumOfBullets()));
        DrawText(700,10, "Missiles: " + ToString(myPc->GetNumOfMissiles()));
        DrawText(700,25, "Mines: " + ToString(myPc->GetNumOfMines()));
        //DrawText(610,450, "ServerPingTime: " + ToString(serverPingTime));
        //DrawText(610,470, "time: " + ToString( (Uint32)GetTime()));// - (Uint32)timeOfLastMessageToServer));
    }

    // Draw ACK list:
    /*
    string ackList;
    ackList += ToString(remoteSequenceNum);
    ackList += "       ";
    for(int i = 0; i < 32; i++) {
    	if(remoteSequenceNumBitField & (unsigned)pow(2, i)) {
    		ackList += '1';
    	}
    	else {
    		ackList += '0';
    	}
    }

    DrawText(10,10, "ACK: " + ackList);
    */


    // Draw list of players and their score:
    DrawText(590, 45, "Players:");
    int pos = 65;
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        if(players[i].state != PLAYER_STATE_DISCONECTED)
        {
            string str(players[i].name);

            str += " - " + ToString(game->GetPlayerCharacterScore(i));

            DrawText(590, pos, str);
            pos += 20;
        }
    }

} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
void Client :: SendDataAsPacket(char data[], int length)
{
    timeOfLastMessageToServer = GetTime();

    // Insert the current local sequence num:
    memcpy(&data[PACKET_SEQUENCE_NUM], &localSequenceNum, 4);

    // Copy the data into a packet:
    memcpy(packets[0]->data, data, length);
    packets[0]->len = length;

    SDLNet_UDP_Send(socket, SERVER, packets[0]);

    StorePacket(data, 100);

    localSequenceNum++;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 Client :: GetLostPacketSequenceNum(Uint32 ackNum, Uint32 ackBitfield)
{
    if( (Uint32)pow(2, 31) & ~ackBitfield )
    {
        return 1;
    }

    return 0;

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: AckSequenceNum(Uint32 newNum)
{
    // If num > remoteSequenceNum
    if(newNum > remoteSequenceNum)
    {

        // shift bitfield left num - remoteSequenceNum
        remoteSequenceNumBitField = remoteSequenceNumBitField << (newNum - remoteSequenceNum);

        // toggle bit coresponding to previous remoteSequenceNum on.
        remoteSequenceNumBitField = remoteSequenceNumBitField | (unsigned)pow(2, newNum - remoteSequenceNum - 1);

        remoteSequenceNum = newNum;
    }
    else if(remoteSequenceNum - newNum <= 32)
    {
        remoteSequenceNumBitField = remoteSequenceNumBitField | (unsigned)pow(2, remoteSequenceNum - newNum - 1);
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: StorePacket(char data[], Uint32 length)
{
    assert(length <= LISTABLE_PACKET_DATA_LENGTH);

    // Push new packet onto queue:
    ListablePacket* packet = new ListablePacket();
    memcpy(&packet->data, data, length);
    packet->length = length;
    sentPacketList.push_front(packet);

    // Pop packets if queue is full:
    Uint32 listSize = sentPacketList.size();
    if(listSize > 50)
    {
        ListablePacket* popPacket;
        popPacket = sentPacketList.back();
        delete popPacket;
        sentPacketList.pop_back();
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
ListablePacket* Client :: GetStoredPacket(Uint32 sequenceNum)
{
    for(std::deque<ListablePacket*>::iterator it = sentPacketList.begin(); it != sentPacketList.end(); it++)
    {
        Uint32 tempSequenceNum;
        memcpy(&tempSequenceNum, &((*it)->data[PACKET_SEQUENCE_NUM]), 4);

        if(tempSequenceNum == sequenceNum)
        {
            return *it;
        }
    }

    return 0;
} // ----------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
void Client :: ReceiveNetworkData()
{
    // Receive n packets of data on our UDP socket:
    int n = SDLNet_UDP_RecvV(socket, packets);

    // For each packet of data we need to look at the first word and determine what to do with it:
    while ( n-- > 0 )
    {
        timeOfLastServerMessage = SDL_GetTicks();

        // Acknowledge packet sequence num:
        Uint32 packetSequenceNum;
        memcpy(&packetSequenceNum, &packets[n]->data[PACKET_SEQUENCE_NUM], 4);
        AckSequenceNum(packetSequenceNum);

        switch (packets[n]->data[0])
        {
        case PACKET_MSG:
            HandleTextMessage(packets[n]);
            break;

        case PACKET_UPDATE_WORLD:
            game->HandleUpdateWorld(packets[n]);
            break;

        case PACKET_LEVEL_DATA:
            game->HandleLevelData(packets[n]);
            break;

        case PACKET_ADD:
            HandleAddNewClient(packets[n]);
            break;

        case PACKET_DEL:
            HandleRemoveClient(packets[n]);
            break;

        case PACKET_ACCEPT_CONNECTION:
            HandleConnectionAccepted(packets[n]);
            break;

        case PACKET_SPAWN:
            HandleSpawn(packets[n]);
            break;

        case PACKET_KILL_NOTIFICATION:
            HandleKillNotification(packets[n]);
            break;
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: SendUpdateServer()
{
    char data[PACKET_PLAYER_UPDATE_SERVER_LENGTH];
    memcpy(&data[0], &PACKET_PLAYER_UPDATE_SERVER, 1);
    memcpy(&data[PACKET_PLAYER_UPDATE_SERVER_SEQUENCE_NUM], &localSequenceNum, 4);
    memcpy(&data[PACKET_PLAYER_UPDATE_SERVER_ACK_NUM], &remoteSequenceNum, 4);
    memcpy(&data[PACKET_PLAYER_UPDATE_SERVER_ACK_BITFIELD], &remoteSequenceNumBitField, 4);

    SendDataAsPacket(data, PACKET_PLAYER_UPDATE_SERVER_LENGTH);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: SendConnectionRequest(string name)
{
    int nameLength = name.length();

    char data[PACKET_REQUEST_CONNECTION_LENGTH + nameLength];
    memcpy(&data[0], &PACKET_REQUEST_CONNECTION, 1);
    memcpy(&data[PACKET_REQUEST_CONNECTION_SEQUENCE_NUM], &localSequenceNum, 4);
    memcpy(&data[PACKET_REQUEST_CONNECTION_NLEN], &nameLength, 4);
    memcpy(&data[PACKET_REQUEST_CONNECTION_NAME], name.c_str(), nameLength);

    SendDataAsPacket(data, PACKET_REQUEST_CONNECTION_LENGTH + nameLength);

    AddMessagetoChat("Requesting Server Connection...");
} // ------------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: SendDisconnectNotification()
{
    char data[PACKET_DISCONNECT_LENGTH];
    memcpy(&data[0], &PACKET_DISCONNECT, 1);
    memcpy(&data[PACKET_DISCONNECT_SEQUENCE_NUM], &localSequenceNum, 4);

    SendDataAsPacket(data, PACKET_DISCONNECT_LENGTH);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: SendTextMessage(string msg)
{
    int msgLength = msg.length();

    char data[PACKET_MSG_LENGTH + msgLength];
    memcpy(&data[0], &PACKET_MSG, 1);
    memcpy(&data[PACKET_MSG_SEQUENCE_NUM], &localSequenceNum, 4);
    memcpy(&data[PACKET_MSG_NLEN], &msgLength, 4);
    memcpy(&data[PACKET_MSG_TEXT], msg.c_str(), msgLength);

    SendDataAsPacket(data,  PACKET_MSG_LENGTH + msgLength);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: SendControlState(ControlState state)
{
    char data[PACKET_PLAYER_CONTROL_STATE_LENGTH];

    memcpy(&data[0], &PACKET_PLAYER_CONTROL_STATE, 1);
    memcpy(&data[PACKET_PLAYER_CONTROL_STATE_SEQUENCE_NUM], &localSequenceNum, 4);
    memcpy(&data[PACKET_PLAYER_CONTROL_STATE_STATE], &state, sizeof(ControlState));

    SendDataAsPacket(data, PACKET_PLAYER_CONTROL_STATE_LENGTH);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: HandleTextMessage(UDPpacket* packet)
{
    Uint32 playerID;
    Uint32 msgLength;

    memcpy(&playerID, &packet->data[PACKET_MSG_PLAYER_ID], 4);
    memcpy(&msgLength, &packet->data[PACKET_MSG_NLEN], 4);

    // Cram the entire message into a string called msg:
    char cmsg[msgLength];
    memcpy(cmsg, &packet->data[PACKET_MSG_TEXT], msgLength);
    string msg(cmsg, 0, msgLength);

    if(playerID == MAX_PLAYERS)
    {
        AddMessagetoChat("Server: " + msg);
    }
    else
    {
        AddMessagetoChat(players[playerID].name + " " + msg);
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: HandleAddNewClient(UDPpacket* packet)
{
    Uint8 channel = packet->data[PACKET_ADD_SLOT];

    // Grab the new clients name an associate with his channel:
    char nameC[100];
    memcpy(nameC, &packet->data[PACKET_ADD_NAME], packet->data[PACKET_ADD_NLEN]);
    players[channel].name.assign(nameC, packet->data[PACKET_ADD_NLEN]);

    // Display a text message:
    AddMessagetoChat(players[channel].name + " has connected on channel: " + ToString(channel));

    // Set the players state to active so that we can begin updating:
    players[channel].state = PLAYER_STATE_ACTIVE;

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: HandleRemoveClient(UDPpacket* packet)
{
    Uint8 channel = packet->data[PACKET_DEL_SLOT];

    // Display a text message:
    AddMessagetoChat(players[channel].name + " has disconnected from channel: " + ToString(channel));

    players[channel].state = PLAYER_STATE_DISCONECTED;

    if(channel == myChannel)
    {
        isConnectedToServer = false;
        exit = true;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: HandleConnectionAccepted(UDPpacket* packet)
{
    // Get channel:
    memcpy(&myChannel, &packet->data[PACKET_ACCEPT_CONNECTION_CHANNEL], 4);

    // Get name:
    players[myChannel].name = clientName;

    // Get pc:
    game->MakeCharacterControllable(myChannel);
    myPc = game->GetPlayerCharacter(myChannel);

    // Display a text message:
    AddMessagetoChat("You have established a conection with the server...");
    isConnectedToServer = true;

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: HandleSpawn(UDPpacket* packet)
{
    Uint32 channel;
    Uint32 posX;
    Uint32 posY;

    memcpy(&channel, &packet->data[PACKET_SPAWN_CHANNEL], 4);
    memcpy(&posX, &packet->data[PACKET_SPAWN_POS_X], 4);
    memcpy(&posY, &packet->data[PACKET_SPAWN_POS_Y], 4);


    //players[channel].pc->Spawn(posX, posY); //FIXTHIS i think this method exists so we can add sparklys here
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Client :: HandleKillNotification(UDPpacket* packet)
{
    Uint32 killerID;
    Uint32 killedID;

    memcpy(&killerID, &packet->data[PACKET_KILL_NOTIFICATION_KILLER_ID], 4);
    memcpy(&killedID, &packet->data[PACKET_KILL_NOTIFICATION_KILLED_ID], 4);

    AddMessagetoChat(players[killedID].name + " was killed by " + players[killerID].name);
} // ----------------------------------------------------------------------------------------------



