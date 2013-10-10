#ifndef PROJECTILE_BASE_H
#define PROJECTILE_BASE_H

#include "SDLWrapper.h"
#include "Vector2df.h"
class GameSim;

typedef float 				Pos;

enum ProjectileType {
	PROJECTILE_TYPE_BULLET,
	PROJECTILE_TYPE_MISSILE,
	PROJECTILE_TYPE_MINE
};

class ProjectileBase {
public:
							ProjectileBase(GameSim* setGameSim);
    virtual                 ~ProjectileBase() {}

	virtual void 			Update(float timeDelta)=0;
	virtual void			Draw(Vector2df camPos) const=0;

	// Modifiers:
	void					SetIsActive(bool b) {isActive = b;}
	void					SetOwnerPlayerID(Uint32 id) {ownerPlayerID = id;}

	// Accessors:
	bool					IsActive() const {return isActive;}
	Uint32					GetOwnerPlayerID() const {return ownerPlayerID;}

protected:
	void					SetHeading(Uint32 d);
	void					SetHeading(Vector2df heading);

	Uint32					GetHeadingDeg() const {return headingDeg;}
	Vector2df				GetHeading() const;

	// Movement data members:
	Vector2df				pos;
	Uint32					headingDeg;

	// Game data members:
	Uint32					ownerPlayerID;
	bool					isActive;
	GameSim* 				gameSim;
};

#endif

