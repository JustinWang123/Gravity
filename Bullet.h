#ifndef BULLET_H
#define BULLET_H

#include "ProjectileBase.h"

const Uint32 				BULLET_HEIGHT = 2;
const Uint32 				BULLET_WIDTH = 2;
const float 				BULLET_SPEED = 5.0f;
const Uint32				BULLET_DAMAGE = 10;

class Bullet : public ProjectileBase {
public:
	static void				Init();
	static void				Destroy();

							Bullet(Uint32 setBulletId, GameSim* setGameSim);
    virtual                 ~Bullet() {}
	void					Activate(Uint32 setOwnerPlayerID, Vector2df setPos, Vector2df setHeading);

	void 					Update(float timeDelta);
	void					Draw(Vector2df camPos) const;

    Uint32					WriteToPacket(Uint32 dataWritePos, Uint8 data[]) const;
    Uint32					ReadFromPacket(Uint32 dataReadPos, Uint8 data[]);

private:
	// Graphics data members:
	static SDL_Surface* 	image;
	int                     bulletId;
};

#endif
