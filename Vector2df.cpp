#include "Vector2df.h"
#include "SDLWrapper.h"
#include <math.h>


Vector2df :: Vector2df :: Vector2df()
{
    x = 0;
    y = 0;
}

Vector2df :: Vector2df(float setX, float setY)
{
    Set(setX, setY);
}

Vector2df :: Vector2df(float setDeg)
{
    SetDeg(setDeg);
}

Vector2df Vector2df :: Normal() const
{
    return *this / Length();
}

float Vector2df :: Deg() const
{
    return GetAngle(0, 0, x, y);
}

float Vector2df :: Length() const
{
    return sqrt( pow(x,2) + pow(y, 2));
}

bool Vector2df :: IsNormal() const
{
    return Length() > 0.99f && Length() < 1.01f;
}

void Vector2df :: Set(float setX, float setY)
{
    x = setX;
    y = setY;
}

void Vector2df :: SetDeg(float setDeg)
{
    x = cos(setDeg * PI/180);
    y = -sin(setDeg * PI/180);
}
