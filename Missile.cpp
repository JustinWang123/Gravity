#include "Missile.h"
#include "Bullet.h"
#include "Game.h"
#include <math.h>
#include <assert.h>

SDL_Surface* Missile :: image = 0;

// ------------------------------------------------------------------------------------------------
Missile :: Missile(Uint32 setId, GameSim* setGameSim)
	:	ProjectileBase(setGameSim),
        id(setId) {

	// Pre-Conditions
	assert(setId < MAX_MINES && "Missile::Missile setId < MAX_MINES");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Missile :: Init() {
	image = LoadSurface("Surfaces/Missile.png");

	// Post-Conditions
	assert(image && "Missile::Init image");
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
void Missile :: Destroy() {
	FreeSurface(image);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Missile :: Activate(Uint32 setOwnerPlayerID, Vector2df setPos, Vector2df setHeading) {
	// Pre-Conditions:
	assert(setPos.x >= MAP_MIN_POS_X && setPos.x < MAP_MAX_POS_X && 	"Missile::Activate setPos.x >= MAP_MIN_POS_X && setPos.x < MAP_MAX_POS_X");
	assert(setPos.y >= MAP_MIN_POS_Y && setPos.y < MAP_MAX_POS_Y && 	"Missile::Activate setPos.y >= MAP_MIN_POS_Y && setPos.y < MAP_MAX_POS_Y");
	assert(setHeading.IsNormal() && 									"Missile::Activate setHeading.IsNormal()");
	assert(setOwnerPlayerID < MAX_PLAYERS && 							"Missile::Activate setOwnerPlayerID < MAX_PLAYERS");

	ownerPlayerID = setOwnerPlayerID;
	pos = setPos;
	SetHeading(setHeading);
	life = MISSILE_LIFE;
	health = MISSILE_HEALTH;
	isActive = true;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Missile :: Update(float timeDelta) {
	// Pre-Conditions:
	assert(isActive && "Missile::Update isActive");

	if(isActive) {
		Vector2df newPos = pos + GetHeading() * MISSILE_SPEED * timeDelta;

		// check for collision ahead of missile:
		if(gameSim->IsPosSolid(newPos + GetHeading() * MISSILE_RADIUS)
		|| life <= 0) {
			gameSim->CreateExplosion(ownerPlayerID, pos);
			SetIsActive(false);
		}
		else {
			pos = newPos;

			if( ((int)life % 5) == 0) {
				gameSim->CreateSmokeParticle(ownerPlayerID, pos, headingDeg - 180, 1.0f);
			}

			life -= timeDelta;
		}
	}

	// Post-Conditions
	assert(pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X && "Missile::Update pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
	assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && "Missile::Update pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Missile :: Draw(Vector2df camPos) const {
	// Pre-conditions:
	assert(isActive &&	"Missile::Draw isActive");
	assert(image && 	"Missile::Draw image");

	if(isActive) {
		Vector2df screenPos = pos - camPos;
		DrawSurface(screenPos.x - MISSILE_WIDTH / 2, screenPos.y - MISSILE_HEIGHT / 2, image);
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool Missile :: IsSolid(Vector2df atPos) {
	assert(isActive && 												"Missile::IsSolid isActive");
	assert(atPos.x >= MAP_MIN_POS_X && atPos.x < MAP_MAX_POS_X && 	"Missile::IsSolid atPos.x >= MAP_MIN_POS_X && atPos.x < MAP_MAX_POS_X");
	assert(atPos.y >= MAP_MIN_POS_Y && atPos.y < MAP_MAX_POS_Y && 	"Missile::IsSolid atPos.y >= MAP_MIN_POS_Y && atPos.y < MAP_MAX_POS_Y");

	return Vector2df(pos.x - atPos.x, pos.y - atPos.y).Length() < MISSILE_RADIUS;
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
void Missile :: DecreaseHealth(Uint32 amount) {
	assert(isActive && 		"Missile::DecreaseHealth isActive");
	assert(amount > 0 && 	"Missile::DecreaseHealth amount > 0");

	if(amount > health) {
		health = 0;
		gameSim->CreateExplosion(ownerPlayerID, pos);
		SetIsActive(false);
	}
	else {
		health -= amount;
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 Missile :: WriteToPacket(Uint32 dataWritePos, Uint8 data[]) {
	// Pre-Conditions:
	assert(isActive && 											"Missile::WriteToPacket isActive");
	assert(pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X && 	"Missile::WriteToPacket pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
	assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && 	"Missile::WriteToPacket pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");
	assert(ownerPlayerID < MAX_PLAYERS && 						"Missile::WriteToPacket ownerPlayerID < MAX_PLAYERS");

	memcpy(&data[dataWritePos + PACKET_WRITE_MISSILE_ID], &id, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_MISSILE_POSX], &pos.x, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_MISSILE_POSY], &pos.y, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_MISSILE_HEADING], &headingDeg, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_MISSILE_LIFE], &life, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_MISSILE_HEALTH], &health, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_MISSILE_PLAYER_ID], &ownerPlayerID, 4);
	return PACKET_WRITE_MISSILE_LENGTH;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 Missile :: ReadFromPacket(Uint32 dataReadPos, Uint8 data[]) {
	Uint32 setHeadingDeg;

	memcpy(&pos.x, &data[dataReadPos + PACKET_WRITE_MISSILE_POSX], 4);
	memcpy(&pos.y, &data[dataReadPos + PACKET_WRITE_MISSILE_POSY], 4);
	memcpy(&setHeadingDeg, &data[dataReadPos + PACKET_WRITE_MISSILE_HEADING], 4);
	memcpy(&life, &data[dataReadPos + PACKET_WRITE_MISSILE_LIFE], 4);
	memcpy(&health, &data[dataReadPos + PACKET_WRITE_MISSILE_HEALTH], 4);
	memcpy(&ownerPlayerID, &data[dataReadPos + PACKET_WRITE_MISSILE_PLAYER_ID], 4);
	isActive = true;
	SetHeading(setHeadingDeg);

	// Post-Conditions:
	assert(isActive && 											"Missile::ReadFromPacket isActive");
	assert(pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X && 	"Missile::ReadFromPacket pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
	assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && 	"Missile::ReadFromPacket pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");
	assert(ownerPlayerID < MAX_PLAYERS && 						"Missile::ReadFromPacket ownerPlayerID < MAX_PLAYERS");

	return PACKET_WRITE_MISSILE_LENGTH;
} // ----------------------------------------------------------------------------------------------
