#include "Game.h"
#include <math.h>
#include <assert.h>

using namespace std;




// ------------------------------------------------------------------------------------------------
Game :: Game() {

	// Allocate a vector of packets for client messages
	packets = SDLNet_AllocPacketV(4, PACKET_PACKETSIZE);

	nextPacketTime = SDL_GetTicks();


} // ----------------------------------------------------------------------------------------------







// ------------------------------------------------------------------------------------------------
Game :: ~Game() {

} // ----------------------------------------------------------------------------------------------







// ------------------------------------------------------------------------------------------------
void Game :: InputChatMessage() {
	// Already typing a message:
	if(isTypingChatMessage) {
		// Left arrow key to move typing cursor:
		if(keysDown[SDLK_LEFT] && chatMessageCursorPos > 0) {
			chatMessageCursorPos--;
		}
		// Right arrow key to move typing cursor:
		else if(keysDown[SDLK_RIGHT] && chatMessageCursorPos < chatMessage.length()) {
			chatMessageCursorPos++;
		}
		// Backspace to delete chars:
		else if(keysReleased[SDLK_BACKSPACE] && chatMessageCursorPos > 0) {
			chatMessageCursorPos--;
			chatMessage.erase(chatMessageCursorPos);
		}
		// Return to send the message to all:
		else if(keysDown[SDLK_RETURN]) {
			// SendTextMessage(chatMessage);
			isTypingChatMessage = false;
			chatMessage.clear();
		}
		// Input any char to text message:
		else {
			InputCharToChatMessage();
		}
	}
	// Start typing a message:
	else if(keysReleased[SDLK_t]) {
		isTypingChatMessage = true;
		chatMessageCursorPos = 0;
		chatMessage.clear();
	}
} // ------------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Game :: InputCharToChatMessage() {
	for(int i = 32; i <= 122; i++) {
		if(keysReleased[i]) {
			if(chatMessage.length() == 0) {
				chatMessage.append(1, (char)i);
			}
			else {
				chatMessage.insert(chatMessageCursorPos, 1, (char)i);
			}

			chatMessageCursorPos++;
		}
	}
} // ------------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Game :: DrawChatLog() {
	Uint32 line = 0;
	Uint32 chatLogPos = 0;

	if(chatLog.size() > CHAT_LOG_NUM_LINES_VISIBLE) {
		chatLogPos = chatLog.size() - CHAT_LOG_NUM_LINES_VISIBLE;
	}

	for(/**/ ; chatLogPos < chatLog.size(); chatLogPos++) {
		DrawText(CHAT_LOG_POS_X, CHAT_LOG_POS_Y + line * TEXT_SIZE, chatLog[chatLogPos]);
		line++;
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Game :: DrawChatMessage() {
	if(isTypingChatMessage) {
		// Draw the empty chatMessage:
		if(chatMessage.length() == 0) {
			DrawText(CHAT_MESSAGE_POS_X, CHAT_MESSAGE_POS_Y, "|");
		}
		// Draw the typing Message:
		else {
			DrawText(CHAT_MESSAGE_POS_X, CHAT_MESSAGE_POS_Y,
				chatMessage.substr(0, chatMessageCursorPos)
				+ "|"
				+ chatMessage.substr(chatMessageCursorPos, chatMessage.length() - chatMessageCursorPos));
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Game :: AddMessagetoChat(string msg) {
	// Get number of full lines:
	int lines =  msg.length() / CHAT_LOG_LINE_LENGTH;

	// If there is a partial line then add one to lines:
	if( (  msg.length() % CHAT_LOG_LINE_LENGTH) > 0 )
		lines++;

	// Push each line onto the chatLog:
	for(int i = 0; i < lines; i++) {
		chatLog.push_back(msg.substr(i * CHAT_LOG_LINE_LENGTH, CHAT_LOG_LINE_LENGTH));
	}
} // ----------------------------------------------------------------------------------------------

