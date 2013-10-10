#ifndef SMOKE_PARTICLE_H
#define SMOKE_PARTICLE_H

#include "ProjectileBase.h"

const Uint32 				SMOKE_PARTICLE_HEIGHT = 16;
const Uint32 				SMOKE_PARTICLE_WIDTH = 16;
const float 				SMOKE_PARTICLE_SPEED = 0.1f;
const Uint32				SMOKE_PARTICLE_LIFE = 25;

class SmokeParticle : public ProjectileBase{
public:
	static void				Init();
	static void				Destroy();

							SmokeParticle(GameSim* setGameSim);
    virtual                 ~SmokeParticle() {}
	void					Activate(int setOwnerPlayerID, Vector2df setPos, Uint32 setHeadingDeg, float setScale);

	void 					Update(float timeDelta);
	void					Draw(Vector2df camPos) const;

private:
	float					scale;
	float					life;
	static SDL_Surface* 	image;
};
#endif
