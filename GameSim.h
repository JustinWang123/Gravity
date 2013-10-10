#ifndef GAME_SIM_H
#define GAME_SIM_H

#include "SDLWrapper.h"
#include "PlayerCharacter.h"
#include "Bullet.h"
#include "Map.h"
#include "Player.h"
#include "Missile.h"
#include "ExplosionParticle.h"
#include "SmokeParticle.h"
#include "Mine.h"
#include "PickUp.h"

const Uint32 	MAX_PLAYERS = 10;
const Uint32	MAX_BULLETS = 100;
const Uint32 	MAX_MISSILES = 20;
const Uint32 	MAX_EXPLOSION_PARTICLES = 50;
const Uint32	MAX_SMOKE_PARTICLES = 50;
const Uint32 	MAX_MINES = 50;
const Uint32 	MAX_PICK_UPS = 20;
const Uint32    RESPAWN_DELAY = 5000;

class GameSim {
public:
							GameSim();
	virtual					~GameSim();

	virtual void			Update();
	virtual void			Draw(Vector2df camPos);

	void					LoadMap(std::string fileName);

	void 					HandleUpdateWorld(UDPpacket* packet);
	Uint32 					WriteGameStateToPacket(Uint8 data[]);

	void 					HandleLevelData(UDPpacket* packet);
	Uint32 					WriteLevelDataToPacket(Uint8 data[]);

	// TEMPORARY:
	void					MakeCharacterControllable(Uint32 id);
	PlayerCharacter*		GetPlayerCharacter(Uint32 playerId) {return playerCharacters[playerId];}
	Uint32					GetPlayerCharacterScore(Uint32 playerId) {return playerCharacters[playerId]->GetScore();}
	// Events:
	void					DamageCharsInCircle(Uint32 attackerID, Uint32 damage, Vector2df centerPos, float radius);
	void					DamageCharsAtPos(Uint32 attackerID, Uint32 damage, Vector2df atPos);

	// Creates:
	void					SpawnPlayerCharacter(Uint32 playerID);
	void					CreateBullet(int ownerPlayerID, Vector2df setPos, Vector2df setHeading);
	void					CreateMissile(int ownerPlayerID, Vector2df setPos, Vector2df setHeading);
	void					CreateMine(int ownerPlayerID, Vector2df setPos, Vector2df setHeading);
	void					CreateExplosion(int ownerPlayerID, Vector2df setPos);
	void					CreateExplosionParticle(int ownerPlayerID, Vector2df setPos, Uint32 headingDeg);
	void					CreateSmokeParticle(int ownerPlayerID, Vector2df setPos, Uint32 headingDeg, float setScale);
	void					CreatePickUp(Vector2df setPos, PickUpType setType);


	// Accessors:
	bool					CheckCollisionWithLevel(Vector2df atPos) const;
	bool					CheckCollisionWithChars(Vector2df atPos) const;
	bool					IsPosSolid(Vector2df atPos) const;
	bool					IsPosGravityWell(Vector2df atPos) const;
	Vector2df				GetPosGravity(Vector2df atPos) const;
	Vector2df				GetClosestCharPos(Vector2df atPos) const;

protected:
	// Update subroutines:
	void					UpdateAllCharacters(float timeDelta);
	void					UpdateAllProjectiles(float timeDelta);
	void					UpdateAllPickUps(float timeDelta);
	void					UpdateRespawn(float timeDelta);

	// Draw subroutines
	void					DrawAllCharacters(Vector2df camPos);
	void					DrawAllProjectiles(Vector2df camPos);
	void					DrawAllPickUps(Vector2df camPos);
	void					DrawMap(Vector2df camPos);

	// Game data members:
	PlayerCharacter*		playerCharacters[MAX_PLAYERS];
	Bullet*					bullets[MAX_BULLETS];
	Missile*				missiles[MAX_MISSILES];
	Mine*					mines[MAX_MINES];
	ExplosionParticle*		explosionParticles[MAX_EXPLOSION_PARTICLES];
	SmokeParticle*			smokeParticles[MAX_SMOKE_PARTICLES];
	PickUp*					pickUps[MAX_PICK_UPS];
	Map*					gameMap;
	Uint32					oldTime;
};
#endif
