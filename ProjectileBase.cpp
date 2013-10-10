#include "ProjectileBase.h"
#include <math.h>
#include "GameSim.h"

// ------------------------------------------------------------------------------------------------
ProjectileBase :: ProjectileBase(GameSim* setGameSim)
	: 	isActive(false),
		gameSim(setGameSim) {
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ProjectileBase :: SetHeading(Uint32 d) {
	headingDeg = d;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ProjectileBase :: SetHeading(Vector2df heading) {
	headingDeg = heading.Deg();
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Vector2df ProjectileBase :: GetHeading() const {
	return Vector2df(headingDeg);
} // ----------------------------------------------------------------------------------------------



