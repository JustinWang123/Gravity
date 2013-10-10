#ifndef MINE_H
#define MINE_H
#include "ProjectileBase.h"
class Game;

const float MINE_SPEED = 2.0f;
const float MINE_TURN_SPEED = 5.0f;
const Uint32 MINE_LIFE = 500;
const Uint32 MINE_ACTIVATION_TIME = 300;
const Uint32 MINE_HEALTH = 30;
const Uint32 MINE_WIDTH = 16;
const Uint32 MINE_HEIGHT = 16;
const Uint32 MINE_RADIUS = 11;
const Uint32 MINE_HOMING_RANGE = 200;

class Mine : public ProjectileBase {
public:
	static void				Init();
	static void				Destroy();

							Mine(Uint32 setId, GameSim* setGameSim);
    virtual                 ~Mine(){}
	void					Activate(Uint32 setOwnerPlayerID, Vector2df setPos, Vector2df setHeading);

	virtual void 			Update(float timeDelta);
	virtual void			Draw(Vector2df camPos) const;

	Uint32                  WriteToPacket(Uint32 dataWritePos, Uint8 data[]);
	Uint32                  ReadFromPacket(Uint32 dataReadPos, Uint8 data[]);

	// Accessors and Mutators:
	void					DecreaseHealth(Uint32 amount);
	void					TurnTowards(Vector2df targetPos);
	bool					IsSolid(Vector2df atPos);

private:
	Uint32                  id;
	bool					isMoving;
	float					life;
	Uint32					health;

	// Graphics data members:
	static SDL_Surface* 	image;
};
#endif

