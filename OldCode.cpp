void Client :: SendUpdate() {
	/*
	char data[512];

	data[0] = CHAT_UPDATE;

	Uint32 x = pc->GetPosX();
	Uint32 y = pc->GetPosY();

	memcpy(&data[CHAT_UPDATE_POSX], &x, 4);
	memcpy(&data[CHAT_UPDATE_POSY], &y, 4);

	// Copy data into a packet:
	memcpy(packets[0]->data, data, CHAT_UPDATE_LENGTH);
	packets[0]->len = CHAT_UPDATE_LENGTH;

	// Send the packet
	SDLNet_UDP_Send(socket, 0, packets[0]);
	*/
}

int Client :: HandleServerData(Uint8 *data)
{
 /*
	int used;

	switch (data[0]) {
		case CHAT_ADD:
			HandleAddNewClient(data);
			used = CHAT_ADD_NAME + data[CHAT_ADD_NLEN];
		break;

		case CHAT_DEL:
			HandleRemoveClient(data);
			used = CHAT_DEL_LEN;
		break;

		case CHAT_BYE:
			printf("* Chat server full\n");
			break;

		case CHAT_MSG:
			HandleMsg(-1, data);
			used = CHAT_BYE_LEN; // Needs to be changed
			break;



		default:
			 Unknown packet type??;
			used = 0;
			break;
	}

	return(used);
	*/
}

void Client :: ReceiveServerData()
{
	/*
	Uint8 data[512];
	int pos;
	int used;

	int len = SDLNet_TCP_Recv(tcpsock, (char *)data, 512);

	// Has the connection been lost with the server?
	if ( len <= 0 ) {
		SDLNet_TCP_DelSocket(socketset, tcpsock);
		SDLNet_TCP_Close(tcpsock);
		tcpsock = NULL;
		printf("Connection with server lost!\n");
	}
	else {
		pos = 0;
		while ( len > 0 ) {
			used = HandleServerData(&data[pos]);
			pos += used;
			len -= used;
			if ( used == 0 ) {
				// We might lose data here.. oh well, we got a corrupt packet from server

				len = 0;
			}
		}
	}
	*/
}


void Server :: ReceiveServerData() {
	/*
	TCPsocket newsocket;
	int slot;
	unsigned char data;

	// Accept an incoming connection on the server TCPsocket.
	// Server sockets are never connected to a remote host.
	// What you get back is a new TCPsocket that is connected to the remote host.
	newsocket = SDLNet_TCP_Accept(serverSocket);
	if ( newsocket == NULL ) {
		return;
	}

	// Look for unconnected person slot
	for ( slot = 0; slot < CHAT_MAXPEOPLE; ++slot ) {
		if ( !people[slot].socket ) {
			break;
		}
	}

	// If the server is full we need to check for connected but inactive people
	// If we find a connected but inactive person then we kick them to make room
	// for the new connection.
	if ( slot == CHAT_MAXPEOPLE ) {
		for ( slot=0; slot<CHAT_MAXPEOPLE; ++slot ) {
			if ( people[slot].socket && !people[slot].active ) {
				// Kick them out..
				data = CHAT_BYE;
				SDLNet_TCP_Send(people[slot].socket, &data, 1);
				SDLNet_TCP_DelSocket(socketSet, people[slot].socket);
				SDLNet_TCP_Close(people[slot].socket);

				fprintf(stderr, "Killed inactive socket %d\n", slot);

				break;
			}
		}
	}

	if ( slot == CHAT_MAXPEOPLE ) {
		// No more room...
		data = CHAT_BYE;
		SDLNet_TCP_Send(newsocket, &data, 1);
		SDLNet_TCP_Close(newsocket);

		fprintf(stderr, "Connection refused -- chat room full\n");
	} else {
		// Add socket as an inactive person
		people[slot].socket = newsocket;
		people[slot].ipAddress = *SDLNet_TCP_GetPeerAddress(newsocket);
		SDLNet_TCP_AddSocket(socketSet, people[slot].socket);

		fprintf(stderr, "New inactive socket %d\n", slot);
	}

	*/
}

void Server :: ReceiveClientData(int which)
{
/*
	char data[512];
	int i;

	// Has the connection been closed?
	if ( SDLNet_TCP_Recv(people[which].socket, data, 512) <= 0 ) {

		fprintf(stderr, "Closing socket %d (was%s active)\n", which, people[which].active ? "" : " not");

		// Notify all active clients
		if ( people[which].active ) {
			people[which].active = 0;
			data[0] = CHAT_DEL;
			data[CHAT_DEL_SLOT] = which;
			for ( i=0; i<CHAT_MAXPEOPLE; ++i ) {
				if ( people[i].active ) {
					SDLNet_TCP_Send(people[i].socket,data,CHAT_DEL_LEN);
				}
			}
		}

		// Delete the closed socket:
		SDLNet_TCP_DelSocket(socketSet, people[which].socket);
		SDLNet_TCP_Close(people[which].socket);
		people[which].socket = NULL;
	}
	else {
		switch (data[0]) {
			case CHAT_HELLO: {

				}
			break;
			default: {
				// Unknown packet type?? ;
			}
			break;
		}
	}
	*/
}

