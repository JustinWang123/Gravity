#ifndef EXPLOSION_PARTICLE_H
#define EXPLOSION_PARTICLE_H

#include "ProjectileBase.h"

const Uint32 				EXPLOSION_PARTICLE_HEIGHT = 16;
const Uint32 				EXPLOSION_PARTICLE_WIDTH = 16;
const float 				EXPLOSION_PARTICLE_SPEED = 2.0f;
const Uint32				EXPLOSION_PARTICLE_DAMAGE = 10;
const Uint32				EXPLOSION_PARTICLE_LIFE = 20;

class ExplosionParticle : public ProjectileBase{
public:
	static void				Init();
	static void				Destroy();

							ExplosionParticle(Uint32 setId, GameSim* setGameSim);
    virtual                 ~ExplosionParticle(){}
	void					Activate(Uint32 setOwnerPlayerID, Vector2df setPos, Uint32 setHeadingDeg);

	void 					Update(float timeDelta);
	void					Draw(Vector2df camPos) const;

	Uint32                  WriteToPacket(Uint32 dataWritePos, Uint8 data[]);
    Uint32                  ReadFromPacket(Uint32 dataReadPos, Uint8 data[]);

private:
	float					life;
	static SDL_Surface* 	image;
	Uint32                  id;
};
#endif
