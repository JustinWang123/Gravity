#include "GameServer.h"
#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include "PacketProtocol.h"



// ------------------------------------------------------------------------------------------------
GameServer :: GameServer()
{
    // Create the socket:
    socket = SDLNet_UDP_Open(NETWORK_PORT);

    if (!socket)
    {
        AddMessagetoChat("Couldn't create server socket");
    }
    else
    {
        AddMessagetoChat("Created server socket");
    }

    // No players are active at first
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        players[i].state = PLAYER_STATE_DISCONECTED;
    }

    std::ifstream serverFile ("data/ServerInit.txt");
    assert(serverFile.is_open());
    std::string str;
    std::string mapName;
    serverFile >> str;
    assert(str == "MapName:" && "Somethings wrong with data/ServerInit.txt");
    serverFile >> mapName;
    serverFile.close();


    numOfPlayers = 0;
    LoadMap(mapName.c_str());
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
GameServer :: ~GameServer()
{
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
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: Update()
{
    ReceiveNetworkData();

    GameBase::Update();

    // Send network data:
    if(SDL_GetTicks() > nextPacketTime)
    {
        SendWorldUpdateToAll();
        nextPacketTime = SDL_GetTicks() + PACKET_SEND_DELAY;
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: Draw()
{
    DrawChatLog();
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: SendDataAsPacket(int to, char data[], int length)
{
    // Insert the current local sequence num:
    memcpy(&data[PACKET_SEQUENCE_NUM], &players[to].localSequenceNum, 4);

    // Copy the data into a packet:
    memcpy(packets[0]->data, data, length);
    packets[0]->len = length;

    // Send the packet:
    // Intentionally lose roughly every 100th packet in order to test ACK
    //if( (rand()%101) != 0) {
    SDLNet_UDP_Send(socket, to, packets[0]);
    //}

    StorePacket(to, data, 100);

    players[to].localSequenceNum++;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: AckSequenceNum(Uint32 playerId, Uint32 newNum)
{
    // If num > remoteSequenceNum
    if(newNum > players[playerId].remoteSequenceNum)
    {

        // shift bitfield left num - remoteSequenceNum
        players[playerId].remoteSequenceNumBitField = players[playerId].remoteSequenceNumBitField << (newNum - players[playerId].remoteSequenceNum);

        // toggle bit coresponding to previous remoteSequenceNum on.
        players[playerId].remoteSequenceNumBitField = players[playerId].remoteSequenceNumBitField | (unsigned)pow(2, newNum - players[playerId].remoteSequenceNum - 1);

        players[playerId].remoteSequenceNum = newNum;
    }
    else if(players[playerId].remoteSequenceNum - newNum <= 32)
    {
        players[playerId].remoteSequenceNumBitField = players[playerId].remoteSequenceNumBitField | (unsigned)pow(2, players[playerId].remoteSequenceNum - newNum - 1);
    }
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
void GameServer :: StorePacket(Uint32 playerId, char data[], Uint32 length)
{
    assert(length <= LISTABLE_PACKET_DATA_LENGTH);

    // Push new packet onto queue:
    ListablePacket* packet = new ListablePacket();
    memcpy(&packet->data, data, length);
    packet->length = length;
    players[playerId].sentPacketList.push_front(packet);

    // Pop packets if queue is full:
    Uint32 listSize = players[playerId].sentPacketList.size();
    if(listSize > 50)
    {
        ListablePacket* popPacket;
        popPacket = players[playerId].sentPacketList.back();
        delete popPacket;
        players[playerId].sentPacketList.pop_back();
    }
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
ListablePacket* GameServer :: GetStoredPacket(Uint32 playerId, Uint32 sequenceNum)
{
    for(std::deque<ListablePacket*>::iterator it = players[playerId].sentPacketList.begin(); it != players[playerId].sentPacketList.end(); it++)
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
void GameServer :: ReceiveNetworkData()
{
    // Receive n packets of data on our UDP socket:
    int n = SDLNet_UDP_RecvV(socket, packets);


    // For each packet of data we need to look at the first word and determine what to do with it:
    while ( n-- > 0 )
    {

        // Note: a channel of -1 denotes an unasigned channel;
        if(packets[n]->channel >= 0 && packets[n]->channel < (Sint32)MAX_PLAYERS)
        {
            players[packets[n]->channel].timeOfLastMessage = GetTime();
        }

        switch (packets[n]->data[0])
        {
        case PACKET_REQUEST_CONNECTION:
            HandleConnectionRequest(packets[n]);
            break;

        case PACKET_DISCONNECT:
            HandleDisconnectNotification(packets[n]);
            break;

        case PACKET_MSG:
            HandleMsg(packets[n]);
            break;

        case PACKET_PLAYER_CONTROL_STATE:
            HandleControlState(packets[n]);
            break;

        case PACKET_PLAYER_UPDATE_SERVER:
            HandlePlayerUpdate(packets[n]);
            break;

        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: DisconnectPlayer(Uint32 playerID)
{
    if(playerID >= 0 && playerID < MAX_PLAYERS && players[playerID].state == PLAYER_STATE_ACTIVE)
    {
        // Notify everyone including the player being disconnected:
        for(Uint32 i = 0; i < MAX_PLAYERS; i++)
        {
            if(players[i].state == PLAYER_STATE_ACTIVE)
            {
                SendDisconnectNotification(i, playerID);
            }
        }

        players[playerID].state = PLAYER_STATE_DISCONECTED;

        numOfPlayers--;

        AddMessagetoChat("Player on channel " + ToString(playerID) + " has disconected...");
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: HandleConnectionRequest(UDPpacket* packet)
{

    // If the game is full then attempt to remove inactive players:
    if(numOfPlayers == MAX_PLAYERS)
    {
        for(Uint32 playerID = 0; playerID < MAX_PLAYERS; playerID++)
        {
            if(GetTime() - players[playerID].timeOfLastMessage > PLAYER_TIME_OUT)
            {
                DisconnectPlayer(playerID);
            }
        }
    }

    bool connectionSuccessful = false;
    for(Uint32 channel = 0; channel < MAX_PLAYERS; channel++)
    {
        if(players[channel].state == PLAYER_STATE_DISCONECTED)
        {
            connectionSuccessful = true;
            numOfPlayers++;

            // Setup a players for this new GameClient:
            players[channel].state = PLAYER_STATE_ACTIVE;
            players[channel].ipAddress.host = packet->address.host;
            players[channel].ipAddress.port = packet->address.port;

            // Get Name:
            char nameC[100];
            memcpy(nameC, &packet->data[PACKET_REQUEST_CONNECTION_NAME], packet->data[PACKET_REQUEST_CONNECTION_NLEN]);
            players[channel].name.assign(nameC, packet->data[PACKET_REQUEST_CONNECTION_NLEN]);

            // Bind the GameClients channel to its ipAddress:
            SDLNet_UDP_Bind(socket, channel, &players[channel].ipAddress);

            // Notify all active GameClients about this new GameClient
            for (Uint32 i = 0; i < MAX_PLAYERS; i++)
            {
                if (players[i].state != PLAYER_STATE_DISCONECTED)
                {
                    SendNew(channel, i);
                }
            }

            // Notify this new GameClient about all other active GameClients
            for (Uint32 i = 0; i < MAX_PLAYERS; i++)
            {
                if (players[i].state != PLAYER_STATE_DISCONECTED && i != channel)
                {
                    SendNew(i, channel);
                }
            }

            AddMessagetoChat("New player added on channel " + ToString(channel) + "...");
            SendTextMessage(channel, MAX_PLAYERS, "Welcome to the server!");
            SendConnectionAccepted(channel);
            SendLevelData(channel);
            SendLevelData(channel);// FIXTHIS SRSLY?
            SendLevelData(channel);
            SpawnPlayerCharacter(channel);
            break;
        }
    }

    if(!connectionSuccessful)
    {
    }

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: HandleDisconnectNotification(UDPpacket* packet)
{
    DisconnectPlayer(packet->channel);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: HandleMsg(UDPpacket* packet)
{
    Uint32 playerID = packet->channel;
    Uint32 msgLength;

    memcpy(&msgLength, &packet->data[PACKET_MSG_NLEN], 4);

    // Extract the message:
    char cmsg[msgLength];
    memcpy(cmsg, &packet->data[PACKET_MSG_TEXT], msgLength);
    string msg(cmsg, 0, msgLength);

    // Send the message to all active GameClients:
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        if(players[i].state != PLAYER_STATE_DISCONECTED)
        {
            SendTextMessage(i, playerID, msg);
        }
    }

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: HandleControlState(UDPpacket* packet)
{
    int channel = packet->channel;

    if(players[channel].state == PLAYER_STATE_ACTIVE)
    {
        ControlState newControlState;
        memcpy(&newControlState, &packet->data[PACKET_PLAYER_CONTROL_STATE_STATE], sizeof(ControlState));
        GetPlayerCharacter(channel)->SetControlState(newControlState);
    }


} // ----------------------------------------------------------------------------------------------








// ------------------------------------------------------------------------------------------------
void GameServer :: HandlePlayerUpdate(UDPpacket* packet)
{
    int playerId = packet->channel;


    Uint32 ackNum;
    Uint32 ackBitfield;

    memcpy(&ackNum, &packet->data[PACKET_PLAYER_UPDATE_SERVER_ACK_NUM], 4);
    memcpy(&ackBitfield, &packet->data[PACKET_PLAYER_UPDATE_SERVER_ACK_BITFIELD], 4);

    // If the player has not acknowledged the message sent 32 messages ago then resend it
    if( !(ackBitfield & (Uint32)pow(2, 31)) )
    {
        ListablePacket* packet = GetStoredPacket(playerId, ackNum - 32);

        if(packet != 0)
        {
            if((Uint32)packet->data[0] != PACKET_UPDATE_WORLD)
            {
                SendTextMessage(playerId, MAX_PLAYERS, "I confirm that you did not receive packet " + ToString(ackNum - 32) + " and this shits important");
                SendDataAsPacket(playerId, packet->data, 100);
            }
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: SendNew(int about, int to)
{
    int nameLength = players[about].name.length();

    char data[PACKET_ADD_LENGTH + nameLength];
    memcpy(&data[0], &PACKET_ADD, 1);
    memcpy(&data[PACKET_ADD_SEQUENCE_NUM], &players[to].localSequenceNum, 4);
    memcpy(&data[PACKET_ADD_SLOT], &about, 4);
    memcpy(&data[PACKET_ADD_NLEN], &nameLength, 4);
    memcpy(&data[PACKET_ADD_NAME], players[about].name.c_str(), nameLength);

    SendDataAsPacket(to, data, PACKET_ADD_LENGTH + players[about].name.length());
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: SendConnectionAccepted(int to)
{
    char data[PACKET_ACCEPT_CONNECTION_LENGTH];
    memcpy(&data[0], &PACKET_ACCEPT_CONNECTION, 1);
    memcpy(&data[PACKET_ACCEPT_CONNECTION_SEQUENCE_NUM], &players[to].localSequenceNum, 4);
    memcpy(&data[PACKET_ACCEPT_CONNECTION_CHANNEL], &to, 4);

    SendDataAsPacket(to, data, PACKET_ACCEPT_CONNECTION_LENGTH);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: SendDisconnectNotification(int to, int about)
{
    char data[PACKET_DEL_LENGTH];
    memcpy(&data[0], &PACKET_DEL, 1);
    memcpy(&data[PACKET_DEL_SEQUENCE_NUM], &players[to].localSequenceNum, 4);
    memcpy(&data[PACKET_DEL_SLOT], &about, 4);

    SendDataAsPacket(to, data, PACKET_DEL_LENGTH);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: SendTextMessage(int to, int from, string msg)
{
    Uint32 msgLength =  msg.length();

    char data[PACKET_MSG_LENGTH + msgLength];
    memcpy(&data[0], &PACKET_MSG, 1);
    memcpy(&data[PACKET_MSG_SEQUENCE_NUM], &players[to].localSequenceNum, 4);
    memcpy(&data[PACKET_MSG_PLAYER_ID], &from, 4);
    memcpy(&data[PACKET_MSG_NLEN], &msgLength, 4);
    memcpy(&data[PACKET_MSG_TEXT], msg.c_str(), msgLength);

    SendDataAsPacket(to, data, PACKET_MSG_LENGTH + msgLength);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: SendWorldUpdateToAll()
{
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        if(players[i].state == PLAYER_STATE_ACTIVE)
        {
            SendWorldUpdate(i);
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: SendWorldUpdate(int to)
{
    Uint8 data[4000];

    // Insert the packet ID:
    data[0] = PACKET_UPDATE_WORLD;

    // Insert the sequence num:
    memcpy(&data[PACKET_UPDATE_WORLD_SEQUENCE_NUM], &players[to].localSequenceNum,4 );

    // Insert a time stamp:
    long time = GetTime();
    memcpy(&data[PACKET_UPDATE_WORLD_TIME_STAMP], &time,4 );

    Uint32 dataWritePos = WriteGameStateToPacket(data);

    SendDataAsPacket(to, (char*)data, dataWritePos);
} // ----------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
void GameServer :: SendLevelData(int to)
{

    Uint8 data[PACKET_LEVEL_DATA_LENGTH];
    memcpy(&data[0], &PACKET_LEVEL_DATA, 1 );

    memcpy(&data[PACKET_LEVEL_DATA_SEQUENCE_NUM], &players[to].localSequenceNum,4 );

    Uint32 dataWritePos = WriteLevelDataToPacket(data);

    SendDataAsPacket(to, (char*)data, dataWritePos);


//	Uint8 data[PACKET_LEVEL_DATA_LENGTH];
    //memcpy(&data[0], &PACKET_LEVEL_DATA, 1 );
    //SendDataAsPacket(to, (char*)data, PACKET_LEVEL_DATA_LENGTH);

} // ----------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
void GameServer :: SendSpawn(int to, int about)
{
    Uint32 posX = GetPlayerCharacter(about)->GetPos().x;
    Uint32 posY = GetPlayerCharacter(about)->GetPos().y;

    char data[PACKET_SPAWN_LENGTH];
    memcpy(&data[0], &PACKET_SPAWN, 1);
    memcpy(&data[PACKET_SPAWN_SEQUENCE_NUM], &players[to].localSequenceNum, 4);
    memcpy(&data[PACKET_SPAWN_CHANNEL], &about, 4);
    memcpy(&data[PACKET_SPAWN_POS_X], &posX, 4);
    memcpy(&data[PACKET_SPAWN_POS_Y], &posY, 4);

    SendDataAsPacket(to, data, PACKET_SPAWN_LENGTH);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: SendKillMessage(int to, int killerPlayerID, int deadPlayerID)
{
    char data [PACKET_KILL_NOTIFICATION_LENGTH];
    memcpy(&data[0], &PACKET_KILL_NOTIFICATION, 1);
    memcpy(&data[PACKET_KILL_NOTIFICATION_SEQUENCE_NUM], &players[to].localSequenceNum, 4);
    memcpy(&data[PACKET_KILL_NOTIFICATION_KILLER_ID], &killerPlayerID, 4);
    memcpy(&data[PACKET_KILL_NOTIFICATION_KILLED_ID], &deadPlayerID, 4);

    SendDataAsPacket(to, data, PACKET_KILL_NOTIFICATION_LENGTH);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameServer :: SendKillMessageToAll(int killerPlayerID, int deadPlayerID)
{
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        if(players[i].state == PLAYER_STATE_ACTIVE)
        {
            SendKillMessage(i, killerPlayerID, deadPlayerID);
        }
    }
} // ----------------------------------------------------------------------------------------------



