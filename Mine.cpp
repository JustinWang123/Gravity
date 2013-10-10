#include "Mine.h"
#include "Bullet.h"
#include "Game.h"
#include <math.h>
#include <assert.h>

SDL_Surface* Mine :: image = 0;

// ------------------------------------------------------------------------------------------------
Mine :: Mine(Uint32 setId, GameSim* setGameSim)
	:	ProjectileBase(setGameSim),
        id(setId) {

	// Pre-Conditions
	assert(setId < MAX_MINES && "Mine::Mine setId < MAX_MINES");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Mine :: Init() {
	image = LoadSurface("Surfaces/Mine.png");

	// Post-Conditions
	assert(image && "Mine::Init image");
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
void Mine :: Destroy() {
	FreeSurface(image);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Mine :: Activate(Uint32 setOwnerPlayerID, Vector2df setPos, Vector2df setHeading) {
	// Pre-Conditions:
	assert(setPos.x >= MAP_MIN_POS_X && setPos.x < MAP_MAX_POS_X && 	"Mine::Activate setPos.x >= MAP_MIN_POS_X && setPos.x < MAP_MAX_POS_X");
	assert(setPos.y >= MAP_MIN_POS_Y && setPos.y < MAP_MAX_POS_Y && 	"Mine::Activate setPos.y >= MAP_MIN_POS_Y && setPos.y < MAP_MAX_POS_Y");
	assert(setHeading.IsNormal() && 									"Mine::Activate setHeading.IsNormal()");
	assert(setOwnerPlayerID < MAX_PLAYERS && 							"Mine::Activate setOwnerPlayerID < MAX_PLAYERS");

	ownerPlayerID = setOwnerPlayerID;
	SetHeading(setHeading);
	pos = setPos;
	life = MINE_LIFE;
	health = MINE_HEALTH;
	isActive = true;
	isMoving = true;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Mine :: Update(float timeDelta) {
	// Pre-Conditions:
	assert(isActive && "Mine::Update isActive");

	if(isActive){
		if(isMoving) {
			Vector2df newPos = pos + GetHeading() * MINE_SPEED * timeDelta;

			// check for map collision ahead of mine and stop it if there is:
			if(gameSim->CheckCollisionWithLevel(newPos + GetHeading() * MINE_RADIUS)) {
				isMoving = false;
			}
			// check for character collision ahead of mine or life running out:
			else if(gameSim->CheckCollisionWithChars(pos + GetHeading() * (MINE_RADIUS + 1))
			|| life <= 0) {
				gameSim->CreateExplosion(ownerPlayerID, pos);
				SetIsActive(false);
			}
			else {
				pos = newPos;
			}
		}
		else if(life < MINE_ACTIVATION_TIME) {
			Vector2df closestCharPos = gameSim->GetClosestCharPos(pos);

			if(Vector2df(closestCharPos.x - pos.x, closestCharPos.y - pos.y).Length() < MINE_HOMING_RANGE) {
				SetHeading(closestCharPos - pos);
				isMoving = true;
			}
		}

		life -= timeDelta;
	}

	// Post-Conditions
	assert(pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X && "Mine::Update pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
	assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && "Mine::Update pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Mine :: Draw(Vector2df camPos) const {
	// Pre-conditions:
	assert(isActive &&	"Mine::Draw isActive");
	assert(image && 	"Mine::Draw image");

	if(IsActive()) {
		Vector2df screenPos = pos - camPos;
		DrawSurface(screenPos.x - MINE_WIDTH / 2, screenPos.y - MINE_HEIGHT / 2, image);
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Mine :: TurnTowards(Vector2df targetPos) {
	assert(isActive && 														"Mine::TurnTowards isActive");
	assert(targetPos.x >= MAP_MIN_POS_X && targetPos.x < MAP_MAX_POS_X && 	"Mine::TurnTowards targetPos.x >= MAP_MIN_POS_X && targetPos.x < MAP_MAX_POS_X");
	assert(targetPos.y >= MAP_MIN_POS_Y && targetPos.y < MAP_MAX_POS_Y && 	"Mine::TurnTowards targetPos.y >= MAP_MIN_POS_Y && targetPos.y < MAP_MAX_POS_Y");

	Uint32 targetHeadingDeg = GetAngle(pos.x, pos.y, targetPos.x, targetPos.y);

	Sint32 MoveDir = 0;
	Sint32 BehindMe = headingDeg - 180;

	if(BehindMe < 0) {
		BehindMe += 360;
	}

	if(abs(targetHeadingDeg - headingDeg) < MINE_TURN_SPEED
	|| abs(targetHeadingDeg - headingDeg) > 360 - MINE_TURN_SPEED) {
		SetHeading(targetHeadingDeg);
	}
	else {

		if (targetHeadingDeg != headingDeg)
		{
			if (headingDeg <= 180)
			{
				if (targetHeadingDeg > headingDeg && (Sint32)targetHeadingDeg < BehindMe)
					MoveDir = 1;
				else
					MoveDir = -1;
			}
			else
			{
				if (targetHeadingDeg < headingDeg && (Sint32)targetHeadingDeg > BehindMe)
					MoveDir = -1;
				else
					MoveDir = 1;
			}

			SetHeading(headingDeg + MoveDir * MINE_TURN_SPEED);
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool Mine :: IsSolid(Vector2df atPos) {
	assert(isActive && 												"Mine::IsSolid isActive");
	assert(atPos.x >= MAP_MIN_POS_X && atPos.x < MAP_MAX_POS_X && 	"Mine::IsSolid atPos.x >= MAP_MIN_POS_X && atPos.x < MAP_MAX_POS_X");
	assert(atPos.y >= MAP_MIN_POS_Y && atPos.y < MAP_MAX_POS_Y && 	"Mine::IsSolid atPos.y >= MAP_MIN_POS_Y && atPos.y < MAP_MAX_POS_Y");

	return Vector2df(pos.x - atPos.x, pos.y - atPos.y).Length() < MINE_RADIUS;
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
void Mine :: DecreaseHealth(Uint32 amount) {
	assert(isActive && 		"Mine::DecreaseHealth isActive");
	assert(amount > 0 && 	"Mine::DecreaseHealth amount > 0");

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
Uint32 Mine :: WriteToPacket(Uint32 dataWritePos, Uint8 data[]) {
	// Pre-Conditions:
	assert(isActive && 											"Mine::WriteToPacket isActive");
	assert(pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X && 	"Mine::WriteToPacket pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
	assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && 	"Mine::WriteToPacket pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");
	assert(ownerPlayerID < MAX_PLAYERS && 						"Mine::WriteToPacket ownerPlayerID < MAX_PLAYERS");

	memcpy(&data[dataWritePos + PACKET_WRITE_MINE_ID], &id, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_MINE_POSX], &pos.x, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_MINE_POSY], &pos.y, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_MINE_HEADING], &headingDeg, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_MINE_LIFE], &life, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_MINE_HEALTH], &health, 4);
	memcpy(&data[dataWritePos + PACKET_WRITE_MINE_PLAYER_ID], &ownerPlayerID, 4);

	return PACKET_WRITE_MINE_LENGTH;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 Mine :: ReadFromPacket(Uint32 dataReadPos, Uint8 data[]) {
	Uint32 setHeadingDeg;

	memcpy(&pos.x, &data[dataReadPos + PACKET_WRITE_MINE_POSX], 4);
	memcpy(&pos.y, &data[dataReadPos + PACKET_WRITE_MINE_POSY], 4);
	memcpy(&setHeadingDeg, &data[dataReadPos + PACKET_WRITE_MINE_HEADING], 4);
	memcpy(&life, &data[dataReadPos + PACKET_WRITE_MINE_LIFE], 4);
	memcpy(&health, &data[dataReadPos + PACKET_WRITE_MINE_HEALTH], 4);
	memcpy(&ownerPlayerID, &data[dataReadPos + PACKET_WRITE_MINE_PLAYER_ID], 4);
	SetHeading(setHeadingDeg);
    isActive = true;
	isMoving = true; // Change to get from packet FIXTHIS

	// Post-Conditions:
	assert(isActive && 											"Mine::ReadFromPacket isActive");
	assert(pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X && 	"Mine::ReadFromPacket pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
	assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && 	"Mine::ReadFromPacket pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");
	assert(ownerPlayerID < MAX_PLAYERS && 						"Mine::ReadFromPacket ownerPlayerID < MAX_PLAYERS");

	return PACKET_WRITE_MINE_LENGTH;
} // ----------------------------------------------------------------------------------------------
