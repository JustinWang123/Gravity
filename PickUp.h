#ifndef PICK_UP_H
#define PICK_UP_H

#include "SDLWrapper.h"
#include "Vector2df.h"

const Uint32				PICK_UP_WIDTH = 16;
const Uint32				PICK_UP_HEIGHT = 16;
const Uint32				RESPAWN_TIME = 1000;
const Uint32				PICK_UP_HEALTH_AMOUNT = 25;
const Uint32				PICK_UP_BULLETS_AMOUNT = 25;
const Uint32				PICK_UP_MISSILES_AMOUNT = 10;
const Uint32				PICK_UP_MINES_AMOUNT = 10;

enum PickUpType {
	PICK_UP_TYPE_REPAIR,
	PICK_UP_TYPE_BULLETS,
	PICK_UP_TYPE_MISSILES,
	PICK_UP_TYPE_MINES
};

typedef float 				Pos;

class PickUp {
public:
	static void				Init();
	static void				Destroy();

							PickUp(Uint32 setId);
							~PickUp();
    void                    Activate(Vector2df setPos, PickUpType setType);

	void					Update(float timeDelta);
	void 					Draw(Vector2df camPos) const;

    Uint32                  WriteToPacket(Uint32 dataWritePos, Uint8 data[]);
	Uint32					ReadFromPacket(Uint32 dataReadPos, Uint8 data[]);

	// Accessors and Modifiers:
	void					Despawn();
	void					SetIsActive(bool b) {isActive = b;}
	bool 					IsActive() const {return isActive;}
	bool                    IsSpawned() const {return isSpawned;}
	//int						GetRespawnTime() const {return timeUntilRespawn;}
	PickUpType 				GetType() const {return type;}
	bool 					IsSolid(Pos atPosX, Pos atPosY) const;

private:
    Uint32                  id;
	PickUpType 				type;
	bool 					isActive;
	bool                    isSpawned;
	Vector2df				pos;
	float					timeUntilRespawn;

	static SDL_Surface*		repairPickUpSurface;
	static SDL_Surface*		bulletsPickUpSurface;
	static SDL_Surface*		missilesPickUpSurface;
	static SDL_Surface*		minesPickUpSurface;
};

#endif
