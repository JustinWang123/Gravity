#ifndef MISSILE_H
#define MISSILE_H
#include "ProjectileBase.h"
class Game;

const float MISSILE_SPEED = 2.0f;
const float MISSILE_TURN_SPEED = 5.0f;
const Uint32 MISSILE_LIFE = 200;
const Uint32 MISSILE_HEALTH = 30;
const Uint32 MISSILE_WIDTH = 16;
const Uint32 MISSILE_HEIGHT = 16;
const Uint32 MISSILE_RADIUS = 5;


class Missile : public ProjectileBase {
public:
	static void				Init();
	static void				Destroy();

							Missile(Uint32 setId, GameSim* setGameSim);
    virtual                 ~Missile() {}
	void					Activate(Uint32 setOwnerPlayerID, Vector2df setPos, Vector2df setHeading);

	virtual void 			Update(float timeDelta);
	virtual void			Draw(Vector2df camPos) const;

    Uint32					WriteToPacket(Uint32 dataWritePos, Uint8 data[]);
	Uint32					ReadFromPacket(Uint32 dataReadPos, Uint8 data[]);

	// Accessors and Mutators:
	void					DecreaseHealth(Uint32 amount);
	bool					IsSolid(Vector2df atPos);

private:
	Uint32                  id;
	float					life;
	Uint32					health;

	// Graphics data members:
	static SDL_Surface* 	image;
};
#endif
