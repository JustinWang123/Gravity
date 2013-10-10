#ifndef SDL_WRAPPER
#define SDL_WRAPPER

#include "SDL.h"
#include "SDL_net.h"
#include <string>
#include "SDL_rotozoom.h"


const Uint32 				TEXT_SIZE = 12;
const float 				PI = 3.14159f;

extern SDL_Event 			event;
extern bool 				quit;
extern bool 				keysDown[300];
extern bool					keysReleased[300];
extern bool 				buttonsDown[3];
extern bool 				buttonsClicked[3];
extern Uint32 				mouseX;
extern Uint32 				mouseY;
extern Uint32 				screenSizeX;
extern Uint32 				screenSizeY;

void 						InitSDL(Uint32 set_screen_size_x, Uint32 set_screen_size_y, const char* window_name);
void 						CloseSDL();
void						SetWindowSize(Uint32 sizeX, Uint32 sizeY);

// Graphics Methods:
SDL_Surface* 				LoadSurface(const char* file_name);
SDL_Surface* 				LoadSurface(std::string file_name);
void 						FreeSurface(SDL_Surface* image);

void 						DrawSurface(int x, int y, SDL_Surface* source);
void 						DrawText(int x, int y, const char* s);
void 						DrawText(int x, int y, std::string s);
void 						RenderScreen();

std::string 				ToString(Sint32 i);
std::string 				ToString(Uint32 i);
int 						GetAngle(float xpos1, float ypos1, float xpos2, float ypos2);
int							ClosestDirrectionToAngle(int currentAngle, int desiredAngle); // returns -1, 0, 1

	// Input Methods:
void 						HandleInput();

// Network Methods:
TCPsocket 					OpenTCPSocket(IPaddress& ip);

// Timing Methods:
long						GetTime();

#endif
