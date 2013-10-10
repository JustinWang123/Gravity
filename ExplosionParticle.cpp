#include "ExplosionParticle.h"
#include "Game.h"
#include <assert.h>

SDL_Surface* ExplosionParticle :: image = 0;

// ------------------------------------------------------------------------------------------------
ExplosionParticle :: ExplosionParticle(Uint32 setId, GameSim* setGameSim)
	:	ProjectileBase(setGameSim),
        id(setId) {

	assert(setId < MAX_EXPLOSION_PARTICLES && "ExplosionParticle::ExplosionParticle setId < MAX_EXPLOSION_PARTICLES");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ExplosionParticle :: Init() {
	image = LoadSurface("Surfaces/ExplosionParticle.png");

	// Post-Conditions
	assert(image && "ExplosionParticle::Init image");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ExplosionParticle :: Destroy() {
	FreeSurface(image);
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
void ExplosionParticle :: Activate(Uint32 setOwnerPlayerID, Vector2df setPos, Uint32 setHeadingDeg) {
	assert(setPos.x >= MAP_MIN_POS_X && setPos.x < MAP_MAX_POS_X && 	"ExplosionParticle::Activate setPos.x >= MAP_MIN_POS_X && setPos.x < MAP_MAX_POS_X");
	assert(setPos.y >= MAP_MIN_POS_Y && setPos.y < MAP_MAX_POS_Y && 	"ExplosionParticle::Activate setPos.y >= MAP_MIN_POS_Y && setPos.y < MAP_MAX_POS_Y");
	assert(setOwnerPlayerID < MAX_PLAYERS &&						"ExplosionParticle::Activate setOwnerPlayerID < MAX_PLAYERS");

	ownerPlayerID = setOwnerPlayerID;
	pos = setPos;
	SetHeading(setHeadingDeg);
	life = EXPLOSION_PARTICLE_LIFE;
	isActive = true;
} // ------------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ExplosionParticle :: Update(float timeDelta) {
	assert(isActive && "ExplosionParticle::Update isActive");

	if(isActive){
		Vector2df nextPos = pos + GetHeading() * EXPLOSION_PARTICLE_SPEED * timeDelta;

		if(gameSim->IsPosSolid(nextPos)) {
			gameSim->DamageCharsAtPos(ownerPlayerID, EXPLOSION_PARTICLE_DAMAGE, nextPos);
			SetIsActive(false);
		}
		else {
			pos = nextPos;

			life -= timeDelta;

			if((int)life % 5 == 0) {
				gameSim->CreateSmokeParticle(ownerPlayerID, pos, headingDeg, 1.0f);
			}

			if(life <= 0) {
				SetIsActive(false);
			}
		}
	}

	// Post-Conditions
	assert(pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X && "ExplosionParticle::Update pos.x > MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
	assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && "ExplosionParticle::Update pos.y > MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void ExplosionParticle :: Draw(Vector2df camPos) const {
	// Pre-conditions:
	assert(isActive &&	"ExplosionParticle::Draw isActive");
	assert(image && 	"ExplosionParticle::Draw image");

	if(IsActive()) {
		Vector2df screenPos = pos - camPos;
		DrawSurface(screenPos.x - EXPLOSION_PARTICLE_WIDTH / 2, screenPos.y - EXPLOSION_PARTICLE_HEIGHT / 2, rotozoomSurface(image, 0, 1, 0));
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 ExplosionParticle :: WriteToPacket(Uint32 dataWritePos, Uint8 data[]) {
	// Pre-Conditions:
	assert(isActive && 											"ExplosionParticle::WriteToPacket isActive");
	assert(pos.x >= MAP_MIN_POS_X && pos.y < MAP_MAX_POS_X && 	"ExplosionParticle::WriteToPacket pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
	assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && 	"ExplosionParticle::WriteToPacket pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");
	assert(ownerPlayerID < MAX_PLAYERS && 						"ExplosionParticle::WriteToPacket ownerPlayerID < MAX_PLAYERS");

	memcpy(&data[dataWritePos + PACKET_WRITE_EXPLOSION_PARTICLE_ID], &id, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_EXPLOSION_PARTICLE_POSX], &pos.x, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_EXPLOSION_PARTICLE_POSY], &pos.y, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_EXPLOSION_PARTICLE_HEADING], &headingDeg, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_EXPLOSION_PARTICLE_PLAYER_ID], &ownerPlayerID, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_EXPLOSION_PARTICLE_LIFE], &life, 4);
	return PACKET_WRITE_EXPLOSION_PARTICLE_LENGTH;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 ExplosionParticle :: ReadFromPacket(Uint32 dataReadPos, Uint8 data[]) {
	memcpy(&pos.x, &data[dataReadPos + PACKET_WRITE_EXPLOSION_PARTICLE_POSX], 4);
	memcpy(&pos.y, &data[dataReadPos + PACKET_WRITE_EXPLOSION_PARTICLE_POSY], 4);
	memcpy(&headingDeg, &data[dataReadPos + PACKET_WRITE_EXPLOSION_PARTICLE_HEADING], 4);
	memcpy(&ownerPlayerID, &data[dataReadPos + PACKET_WRITE_EXPLOSION_PARTICLE_PLAYER_ID], 4);
	memcpy(&life, &data[dataReadPos + PACKET_WRITE_EXPLOSION_PARTICLE_LIFE], 4);
	isActive = true;
	SetHeading(headingDeg);

	 // Post-Conditions:
	assert(isActive && 											"ExplosionParticle::ReadFromPacket isActive");
	assert(pos.x >= MAP_MIN_POS_X && pos.y < MAP_MAX_POS_X && 	"ExplosionParticle::ReadFromPacket pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
	assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && 	"ExplosionParticle::ReadFromPacket pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");
	assert(ownerPlayerID < MAX_PLAYERS && 						"ExplosionParticle::ReadFromPacket ownerPlayerID < MAX_PLAYERS");

	return PACKET_WRITE_EXPLOSION_PARTICLE_LENGTH;
} // ----------------------------------------------------------------------------------------------
