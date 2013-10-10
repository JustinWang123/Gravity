#include "SmokeParticle.h"
#include "Game.h"

SDL_Surface* SmokeParticle :: image = 0;

// ------------------------------------------------------------------------------------------------
SmokeParticle :: SmokeParticle(GameSim* setGameSim)
	:	ProjectileBase(setGameSim) {
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void SmokeParticle :: Init() {
	image = LoadSurface("Surfaces/SmokeParticle.png");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void SmokeParticle :: Destroy() {
	FreeSurface(image);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void SmokeParticle :: Activate(int setOwnerPlayerID, Vector2df setPos, Uint32 setHeadingDeg, float setScale) {
	ownerPlayerID = setOwnerPlayerID;
	pos = setPos;
	SetHeading(setHeadingDeg);
	life = SMOKE_PARTICLE_LIFE;
	isActive = true;
	scale = setScale;
} // ------------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void SmokeParticle :: Update(float timeDelta) {
	life -= timeDelta;

	pos += GetHeading() * SMOKE_PARTICLE_SPEED * timeDelta;

	if(life <= 0) {
		SetIsActive(false);
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void SmokeParticle :: Draw(Vector2df camPos) const {
	if(IsActive()) {
		Vector2df screenPos = pos - camPos;

		float scaleMod = scale * ((float)life / (float)SMOKE_PARTICLE_LIFE);

		DrawSurface(screenPos.x - (scaleMod * SMOKE_PARTICLE_WIDTH) / 2,
					screenPos.y - (scaleMod * SMOKE_PARTICLE_HEIGHT) / 2,
					rotozoomSurface(image, 0, scaleMod, 0));
	}
} // ----------------------------------------------------------------------------------------------

