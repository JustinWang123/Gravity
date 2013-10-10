#include "Bullet.h"
#include "GameSim.h"
#include "PacketProtocol.h"
#include <assert.h>

SDL_Surface* Bullet :: image = NULL;

// ------------------------------------------------------------------------------------------------
Bullet :: Bullet(Uint32 setBulletId, GameSim* setGameSim)
	:	ProjectileBase(setGameSim),
        bulletId(setBulletId) {

	// Pre-Conditions
	assert(setBulletId < MAX_BULLETS && "Bullet::Bullet setBulletId < MAX_BULLETS");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Bullet :: Init() {
	image = LoadSurface("Surfaces/Bullet.bmp");

	// Post-Conditions
	assert(image && "Bullet::Init image");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Bullet :: Destroy() {
	FreeSurface(image);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Bullet :: Activate(Uint32 setOwnerPlayerID, Vector2df setPos, Vector2df setHeading) {
	// Pre-Conditions:
	assert(setPos.x >= MAP_MIN_POS_X && setPos.x < MAP_MAX_POS_X && 	"Bullet::Activate setPos.x >= MAP_MIN_POS_X && setPos.x < MAP_MAX_POS_X");
	assert(setPos.y >= MAP_MIN_POS_Y && setPos.y < MAP_MAX_POS_Y && 	"Bullet::Activate setPos.y >= MAP_MIN_POS_Y && setPos.y < MAP_MAX_POS_Y");
	assert(setHeading.IsNormal() && 									"Bullet::Activate setHeading.IsNormal()");
	assert(setOwnerPlayerID < MAX_PLAYERS && 							"Bullet::Activate setOwnerPlayerID < MAX_PLAYERS");

	ownerPlayerID = setOwnerPlayerID;
	pos = setPos;
	SetHeading(setHeading);
	isActive = true;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Bullet :: Update(float timeDelta) {
	// Pre-Conditions:
	assert(isActive && "Bullet::Update isActive");

	if(isActive) {
		Vector2df nextPos = pos + GetHeading() * BULLET_SPEED * timeDelta;

		if(gameSim->IsPosSolid(nextPos)) {
			gameSim->DamageCharsAtPos(ownerPlayerID, BULLET_DAMAGE, nextPos);
			SetIsActive(false);
		}
		else {
			pos = nextPos;
		}
	}

	// Post-Conditions
	assert(pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X && "Bullet::Update pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
	assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && "Bullet::Update pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Bullet :: Draw(Vector2df camPos) const {
	// Pre-conditions:
	assert(isActive &&	"Bullet::Draw isActive");
	assert(image && 	"Bullet::Draw image");

	if(IsActive()) {
		Vector2df screenPos = pos - camPos;
		DrawSurface(screenPos.x - BULLET_WIDTH / 2, screenPos.y - BULLET_HEIGHT / 2, image);
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 Bullet :: WriteToPacket(Uint32 dataWritePos, Uint8 data[]) const {
	// Pre-Conditions:
	assert(isActive && 											"Bullet::WriteToPacket isActive");
	assert(pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X && 	"Bullet::WriteToPacket pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
	assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && 	"Bullet::WriteToPacket pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");
	assert(ownerPlayerID < MAX_PLAYERS && 						"Bullet::WriteToPacket ownerPlayerID < MAX_PLAYERS");

	memcpy(&data[dataWritePos + PACKET_WRITE_BULLET_ID], &bulletId, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_BULLET_POSX], &pos.x, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_BULLET_POSY], &pos.y, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_BULLET_HEADING], &headingDeg, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_BULLET_PLAYER_ID], &ownerPlayerID, 4);
	return PACKET_WRITE_BULLET_LENGTH;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 Bullet :: ReadFromPacket(Uint32 dataReadPos, Uint8 data[]) {
    Uint32 setHeadingDeg;
	memcpy(&pos.x, &data[dataReadPos + PACKET_WRITE_BULLET_POSX], 4);
	memcpy(&pos.y, &data[dataReadPos + PACKET_WRITE_BULLET_POSY], 4);
	memcpy(&setHeadingDeg, &data[dataReadPos + PACKET_WRITE_BULLET_HEADING], 4);
	memcpy(&ownerPlayerID, &data[dataReadPos + PACKET_WRITE_BULLET_PLAYER_ID], 4);
	SetHeading(setHeadingDeg);
    isActive = true;

    // Post-Conditions:
	assert(isActive && 											"Bullet::ReadFromPacket isActive");
	assert(pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X && 	"Bullet::ReadFromPacket pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
	assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && 	"Bullet::ReadFromPacket pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");
	assert(ownerPlayerID < MAX_PLAYERS && 						"Bullet::ReadFromPacket ownerPlayerID < MAX_PLAYERS");

    return PACKET_WRITE_BULLET_LENGTH;
} // ------------------------------------------------------------------------------------------------
