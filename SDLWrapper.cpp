#include "SDLWrapper.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include <math.h>
#include <sstream>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <time.h>

Uint32 						screenSizeX;
Uint32 						screenSizeY;
SDL_Surface* 				screen;
Uint32 						colorKey;
Uint32 						drawColor;
SDL_Color 					textColor;
TTF_Font* 					font;
SDL_Event 					event;
bool 						quit;
bool 						keysDown[300];
bool						keysReleased[300];
bool 						buttonsDown[3];
bool 						buttonsClicked [3];
Uint32 						mouseX;
Uint32 						mouseY;


void InitSDL(Uint32 set_screenSizeX, Uint32 set_screenSizeY, const char* window_name)
{
    screenSizeX = set_screenSizeX;
    screenSizeY = set_screenSizeY;
    quit = false;

    for(int i = 0; i < 300; i++)
    {
        keysReleased[i] = false;
        keysDown[i] = false;
    }

    for(int i = 0; i < 3; i++)
        buttonsDown[i] = false;

    //Start SDL:

    /* Initialize SDL */
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
        printf("%s\n", SDL_GetError());
        exit(1);
    }

    /* Initialize the network */
    if ( SDLNet_Init() < 0 )
    {
        fprintf(stderr, "Couldn't initialize net: %s\n", SDLNet_GetError());
        SDL_Quit();
        exit(1);
    }

    SDL_WM_SetCaption( window_name, NULL );
    screen = SDL_SetVideoMode( screenSizeX, screenSizeY, 32, SDL_SWSURFACE);
    TTF_Init();

    // Set color key for transparent colour:
    colorKey = SDL_MapRGB(screen->format, 255, 0, 255);
    drawColor = SDL_MapRGB(screen->format, 255, 255, 255);
    textColor.r = 255;
    textColor.g = 0;
    textColor.b = 0;

    font = TTF_OpenFont( "Surfaces/data-latin.ttf", TEXT_SIZE);
}


void CloseSDL()
{
    //Quit SDL
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

void SetWindowSize(Uint32 sizeX, Uint32 sizeY)
{
    screen = SDL_SetVideoMode(sizeX, sizeY, 32, SDL_SWSURFACE);
}

SDL_Surface* LoadSurface(const char* file_name)
{
    SDL_Surface* tempSurface = 0;
    tempSurface = IMG_Load(file_name);

    assert(tempSurface && "Cannot Load Surface!");

    SDL_Surface* outputSurface = SDL_DisplayFormat(tempSurface);
    SDL_SetColorKey( outputSurface, SDL_SRCCOLORKEY, colorKey );
    SDL_FreeSurface(tempSurface);

    return outputSurface;
}

SDL_Surface* LoadSurface(std::string file_name)
{
    return LoadSurface(file_name.c_str());
}

void DrawSurface(int x, int y, SDL_Surface* source)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface( source, NULL, screen, &offset );
}

void DrawText(int x, int y, const char* s)
{
    SDL_Surface* text = TTF_RenderText_Solid( font, s, textColor );
    DrawSurface(x, y, text);
    SDL_FreeSurface(text);
}

void DrawText(int x, int y, std::string s)
{
    DrawText(x, y, s.c_str());
}

void RenderScreen()
{
    // Update screen:

    SDL_Flip(screen);
    SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );
}

void FreeSurface(SDL_Surface* image)
{

    SDL_FreeSurface(image);
}



void HandleInput()
{
    buttonsClicked[0] = false;
    buttonsClicked[1] = false;
    buttonsClicked[2] = false;

    for(int i = 0; i < 300; i++)
    {
        keysReleased[i] = false;
    }

    //While there's an event to handle
    while( SDL_PollEvent( &event ) )
    {
        if( event.type == SDL_KEYDOWN )
        {
            keysDown[event.key.keysym.sym] = true;
        }
        else if( event.type == SDL_KEYUP )
        {
            keysDown[event.key.keysym.sym] = false;
            keysReleased[event.key.keysym.sym] = true;
        }
        else if( event.type == SDL_MOUSEMOTION )
        {
            mouseX = event.motion.x;
            mouseY = event.motion.y;
        }
        else if( event.type == SDL_MOUSEBUTTONDOWN )
        {
            buttonsDown[event.button.button - 1] = true;
        }
        else if( event.type == SDL_MOUSEBUTTONUP )
        {
            buttonsDown[event.button.button - 1] = false;
            buttonsClicked[event.button.button - 1] = true;
        }
        else if( event.type == SDL_QUIT )
        {
            quit = true;
        }
    }
}

int GetAngle(float x1, float y1, float x2, float y2)
{
    int angle = 0;

    if(y2 < y1)
        angle = (int)(90 - atan((float)(x2 - x1) / (float)(y1 - y2) ) * 180/PI);
    else if(y2 > y1)
        angle = (int)(270 - atan((float)(x2 - x1) / (float)(y1 - y2) ) * 180/PI);
    else
    {
        if(x2 > x1)
            angle = 0;
        else
            angle = 180;
    }

    return angle;
}

std::string ToString(Sint32 i)
{
    std::stringstream out;
    out << i;
    return out.str();
}

std::string ToString(Uint32 i)
{
    std::stringstream out;
    out << i;
    return out.str();
}

TCPsocket OpenTCPSocket(IPaddress& ip)
{
    TCPsocket tcpSocket = NULL;

    if ( ip.host == INADDR_NONE )
    {
        printf("Couldn't resolve hostname\n");
    }
    else
    {
        tcpSocket = SDLNet_TCP_Open(&ip);

        if ( tcpSocket == NULL )
        {
            printf("Connect failed\n");
        }
        else
        {
            printf("Connected\n");
        }
    }

    return tcpSocket;
}

long GetTime()
{
    /*
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = spec.tv_nsec / 1.0e6; // Convert nanoseconds to milliseconds

    return s * 1000 + ms;
    */

    time_t timer;
    time(&timer);  /* get current time; same as: timer = time(NULL)  */

    struct tm y2k;
    y2k.tm_hour = 0;
    y2k.tm_min = 0;
    y2k.tm_sec = 0;
    y2k.tm_year = 110;
    y2k.tm_mon = 0;
    y2k.tm_mday = 1;

    double seconds = difftime(timer, mktime(&y2k));

    return (long)seconds;
}


int	ClosestDirrectionToAngle(int currentAngle, int desiredAngle)
{
    if(currentAngle == desiredAngle)
    {
        return 0;
    }
    else
    {

        int moveDir = 0;
        int BehindMe = currentAngle - 180;

        if(BehindMe < 0)
        {
            BehindMe += 360;
        }

        if (currentAngle <= 180)
        {
            if (desiredAngle > currentAngle && desiredAngle < BehindMe)
                moveDir = 1;
            else
                moveDir = -1;
        }
        else
        {
            if (desiredAngle < currentAngle && desiredAngle > BehindMe)
                moveDir = -1;
            else
                moveDir = 1;
        }

        return moveDir;
    }
}

