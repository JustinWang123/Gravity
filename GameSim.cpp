#include "GameSim.h"
#include "PacketProtocol.h"
#include "PlayerCharacterControlled.h"
#include <assert.h>

using namespace std;

// ------------------------------------------------------------------------------------------------
GameSim :: GameSim() {
	// Init all players:
	for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
		playerCharacters[i] = new PlayerCharacter(i, this);
	}

	// Init all bullets:
	for(Uint32 i = 0; i < MAX_BULLETS; i++) {
		bullets[i] = new Bullet(i, this);
	}

	// init all missiles:
	for(Uint32 i = 0; i < MAX_MISSILES; i++) {
		missiles[i] = new Missile(i, this);
	}

	// init all explosion particles:
	for(Uint32 i = 0; i < MAX_EXPLOSION_PARTICLES; i++) {
		explosionParticles[i] = new ExplosionParticle(i, this);
	}

	// init all smoke particles:
	for(Uint32 i = 0; i < MAX_SMOKE_PARTICLES; i++) {
		smokeParticles[i] = new SmokeParticle(this);
	}

	// init all mines:
	for(Uint32 i = 0; i < MAX_MINES; i++) {
		mines[i] = new Mine(i, this);
	}

	// init all pick ups:
	for(Uint32 i = 0; i < MAX_PICK_UPS; i++) {
		pickUps[i] = new PickUp(i);
	}

	oldTime = SDL_GetTicks();
	gameMap = new Map(Vector2df(4, 4), this);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
GameSim :: ~GameSim() {
	// delete all player characters:
	for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
		delete playerCharacters[i];
		playerCharacters[i] = 0;
	}

	// delete all bullets:
	for(Uint32 i = 0; i < MAX_BULLETS; i++) {
		delete bullets[i];
		bullets[i] = 0;
	}

	// delete all missiles:
	for(Uint32 i = 0; i < MAX_MISSILES; i++) {
		delete missiles[i];
		missiles[i] = 0;
	}

	// delete all explosion particles:
	for(Uint32 i = 0; i < MAX_EXPLOSION_PARTICLES; i++) {
		delete explosionParticles[i];
		explosionParticles[i] = 0;
	}

	// delete all smoke particles:
	for(Uint32 i = 0; i < MAX_SMOKE_PARTICLES; i++) {
		delete smokeParticles[i];
		smokeParticles[i] = 0;
	}

	// delete all mines:
	for(Uint32 i = 0; i < MAX_MINES; i++) {
		delete mines[i];
		mines[i] = 0;
	}

	// delete all pick ups:
	for(Uint32 i = 0; i < MAX_PICK_UPS; i++) {
		if(pickUps[i] != 0) {
			delete pickUps[i];
		}
	}

	delete gameMap;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: Update() {
	float timeDelta = (SDL_GetTicks() - oldTime) / 20.0f;
	oldTime = SDL_GetTicks();

	UpdateAllCharacters(timeDelta);
	UpdateAllProjectiles(timeDelta);
	UpdateAllPickUps(timeDelta);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: Draw(Vector2df camPos) {
	DrawMap(camPos);
	DrawAllCharacters(camPos);
	DrawAllProjectiles(camPos);
	DrawAllPickUps(camPos);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: LoadMap(string fileName) {
	gameMap->LoadMap(fileName);
} // ------------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
Uint32 GameSim :: WriteLevelDataToPacket(Uint8 data[]) {
	return gameMap->WriteLevelDataToPacket(data);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: HandleLevelData(UDPpacket* packet) {
	gameMap->HandleLevelData(packet);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 GameSim :: WriteGameStateToPacket(Uint8 data[]) {
	Uint32 dataWritePos = PACKET_UPDATE_WORLD_DATA_START;

	// Insert all players:
	Uint32 numOfPlayers = 0;
	for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
		if(playerCharacters[i]->IsActive()) {
			dataWritePos += playerCharacters[i]->WriteToPacket(dataWritePos, data);
			numOfPlayers++;
		}
	}
	memcpy(&data[PACKET_UPDATE_WORLD_NUM_OF_PLAYERS], &numOfPlayers, 4 );

	// Insert all bullets:
	Uint32 numOfBullets = 0;
	for(Uint32 i = 0; i < MAX_BULLETS; i++) {
		if(bullets[i]->IsActive()) {
            dataWritePos += bullets[i]->WriteToPacket(dataWritePos, data);
			numOfBullets++;
		}
	}
	memcpy(&data[PACKET_UPDATE_WORLD_NUM_OF_BULLETS], &numOfBullets, 4 );

	// Insert all Missiles:
	Uint32 numOfMissiles = 0;
	for(Uint32 i = 0; i < MAX_MISSILES; i++) {
		if(missiles[i]->IsActive()) {
			dataWritePos += missiles[i]->WriteToPacket(dataWritePos, data);
			numOfMissiles++;
		}
	}
	memcpy(&data[PACKET_UPDATE_WORLD_NUM_OF_MISSILES], &numOfMissiles, 4 );

	// Insert all Mines:
	Uint32 numOfMines = 0;
	for(Uint32 i = 0; i < MAX_MINES; i++) {
		if(mines[i]->IsActive()) {
			dataWritePos += mines[i]->WriteToPacket(dataWritePos, data);
			numOfMines++;
		}
	}
	memcpy(&data[PACKET_UPDATE_WORLD_NUM_OF_MINES], &numOfMines, 4 );

	// Insert all Explosion Particles:
	Uint32 numOfExplosionParticles = 0;
	for(Uint32 i = 0; i < MAX_EXPLOSION_PARTICLES; i++) {
		if(explosionParticles[i]->IsActive()) {
			dataWritePos += explosionParticles[i]->WriteToPacket(dataWritePos, data);
			numOfExplosionParticles++;
		}
	}
	memcpy(&data[PACKET_UPDATE_WORLD_NUM_OF_EXPLOSION_PARTICLES], &numOfExplosionParticles, 4 );

	// Insert all Pick Ups:
	Uint32 numOfPickUps = 0;
	for(Uint32 i = 0; i < MAX_PICK_UPS; i++) {
		if(pickUps[i]->IsActive()) {
			dataWritePos += pickUps[i]->WriteToPacket(dataWritePos, data);
			numOfPickUps++;
		}
	}
	memcpy(&data[PACKET_UPDATE_WORLD_NUM_OF_PICK_UPS], &numOfPickUps, 4 );

	return dataWritePos;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: HandleUpdateWorld(UDPpacket* packet) {

	// FIXTHIS
	// Get server time stamp and calculate ping:
	//long time;
	////memcpy(&time, &packet->data[PACKET_UPDATE_WORLD_TIME_STAMP], 4);
	//Uint32 timeDiff = GetTime() - time;
	//serverPingTime = timeDiff * 2;

	// We set everything to inactive and only activate those things we receive a message about:
	for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
		playerCharacters[i]->SetIsActive(false);
	}

	for(Uint32 i = 0; i < MAX_BULLETS; i++) {
		bullets[i]->SetIsActive(false);
	}

	for(Uint32 i = 0; i < MAX_MISSILES; i++) {
		missiles[i]->SetIsActive(false);
	}

	for(Uint32 i = 0; i < MAX_MINES; i++) {
		mines[i]->SetIsActive(false);
	}

	for(Uint32 i = 0; i < MAX_EXPLOSION_PARTICLES; i++) {
		explosionParticles[i]->SetIsActive(false);
	}

	for(Uint32 i = 0; i < MAX_PICK_UPS; i++) {
		pickUps[i]->SetIsActive(false);
	}



	// Get number (amount) of each data type:
	Uint32 numOfPlayers;
	Uint32 numOfBullets;
	Uint32 numOfMissiles;
	Uint32 numOfMines;
	Uint32 numOfExplosionParticles;
	Uint32 numOfPickUps;
	memcpy(&numOfPlayers, &packet->data[PACKET_UPDATE_WORLD_NUM_OF_PLAYERS], 4);
	memcpy(&numOfBullets, &packet->data[PACKET_UPDATE_WORLD_NUM_OF_BULLETS], 4);
	memcpy(&numOfMissiles, &packet->data[PACKET_UPDATE_WORLD_NUM_OF_MISSILES], 4);
	memcpy(&numOfMines, &packet->data[PACKET_UPDATE_WORLD_NUM_OF_MINES], 4);
	memcpy(&numOfExplosionParticles, &packet->data[PACKET_UPDATE_WORLD_NUM_OF_EXPLOSION_PARTICLES], 4);
	memcpy(&numOfPickUps, &packet->data[PACKET_UPDATE_WORLD_NUM_OF_PICK_UPS], 4);

	// Get all the data:
	Uint32 id;
	Uint32 dataReadPos = PACKET_UPDATE_WORLD_DATA_START;

	// Read Players:
	for(Uint32 i = 0; i < numOfPlayers; i++) {
		memcpy(&id, &packet->data[dataReadPos + PACKET_WRITE_PLAYER_ID], 4);
		dataReadPos += playerCharacters[id]->ReadFromPacket(dataReadPos, packet->data);
	}

    // Read Bullets:
	for(Uint32 i = 0; i < numOfBullets; i++) {
        memcpy(&id, &packet->data[dataReadPos + PACKET_WRITE_BULLET_ID], 4);
		dataReadPos += bullets[id]->ReadFromPacket(dataReadPos, packet->data);
	}

	// Read Missile:
	for(Uint32 i = 0; i < numOfMissiles; i++) {
		memcpy(&id, &packet->data[dataReadPos + PACKET_WRITE_MISSILE_ID], 4);
		dataReadPos += missiles[id]->ReadFromPacket(dataReadPos, packet->data);
	}

	// Read Mines:
	for(Uint32 i = 0; i < numOfMines; i++) {
		memcpy(&id, &packet->data[dataReadPos + PACKET_WRITE_MINE_ID], 4);
		dataReadPos += mines[id]->ReadFromPacket(dataReadPos, packet->data);
	}

    // Read Explosion Particles:
	for(Uint32 i = 0; i < numOfExplosionParticles; i++) {
		memcpy(&id, &packet->data[dataReadPos + PACKET_WRITE_EXPLOSION_PARTICLE_ID], 4);
		dataReadPos += explosionParticles[id]->ReadFromPacket(dataReadPos, packet->data);
	}

	// Read Pick Ups:
	for(Uint32 i = 0; i < numOfPickUps; i++) {
		memcpy(&id, &packet->data[dataReadPos + PACKET_WRITE_PICK_UP_ID], 4);
		dataReadPos += pickUps[id]->ReadFromPacket(dataReadPos, packet->data);
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: MakeCharacterControllable(Uint32 id) {
	if(id < MAX_PLAYERS) {
		delete playerCharacters[id];
		playerCharacters[id] = new PlayerCharacterControlled(id, this);
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: DamageCharsInCircle(Uint32 attackerId, Uint32 damage, Vector2df centerPos, float radius) {
	assert(attackerId < MAX_PLAYERS && "GameSim::DamageCharsInCircle attackerId < MAX_PLAYERS");

	for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
		if(playerCharacters[i]->IsActive()) {
			float distance = (centerPos - playerCharacters[i]->GetPos()).Length();
			if(distance  < radius) {
				playerCharacters[i]->DecreaseHealth(attackerId, damage);
			}
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: DamageCharsAtPos(Uint32 attackerId, Uint32 damage, Vector2df atPos) {
	assert(attackerId < MAX_PLAYERS && "GameSim::DamageCharsAtPos attackerId < MAX_PLAYERS");

	// damage players:
	for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
		if(playerCharacters[i]->IsActive() && playerCharacters[i]->IsSolid(atPos)) {
			playerCharacters[i]->DecreaseHealth(attackerId, damage);
		}
	}

	// damage missiles:
	for(Uint32 i = 0; i < MAX_MISSILES; i++) {
		if(missiles[i]->IsActive() && missiles[i]->IsSolid(atPos)) {
			missiles[i]->DecreaseHealth(damage);
		}
	}

	// damage mines:
	for(Uint32 i = 0; i < MAX_MINES; i++) {
		if(mines[i]->IsActive() && mines[i]->IsSolid(atPos)) {
			mines[i]->DecreaseHealth(damage);
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: SpawnPlayerCharacter(Uint32 playerID) {
	Vector2df spawnPos = gameMap->GetRandomSpawnPointPos();
	playerCharacters[playerID]->Spawn(spawnPos);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: CreateBullet(int ownerPlayerID, Vector2df setPos, Vector2df setHeading) {
	// Find the first bullet that is inactive:
	for(Uint32 i = 0; i < MAX_BULLETS; i++) {
		if(!bullets[i]->IsActive()) {
			bullets[i]->Activate(ownerPlayerID, setPos, setHeading);
			break;
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: CreateMissile(int ownerPlayerID, Vector2df setPos, Vector2df setHeading) {
	// Find the first missile that is inactive:
	for(Uint32 i = 0; i < MAX_MISSILES; i++) {
		if(!missiles[i]->IsActive()) {
			missiles[i]->Activate(ownerPlayerID, setPos, setHeading);
			break;
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: CreateMine(int ownerPlayerID, Vector2df setPos, Vector2df setHeading) {
	// Find the first mine that is inactive:
	for(Uint32 i = 0; i < MAX_MINES; i++) {
		if(!mines[i]->IsActive()) {
			mines[i]->Activate(ownerPlayerID, setPos, setHeading);
			break;
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: CreateExplosion(int ownerPlayerID, Vector2df setPos) {
	for(Uint32 i = 0; i < 360; i += 35) {
		CreateExplosionParticle(ownerPlayerID, setPos, i);
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: CreateExplosionParticle(int ownerPlayerID, Vector2df setPos, Uint32 headingDeg) {
	// Find the first exposion particle that is inactive:
	for(Uint32 i = 0; i < MAX_EXPLOSION_PARTICLES; i++) {
		if(!explosionParticles[i]->IsActive()) {
			explosionParticles[i]->Activate(ownerPlayerID, setPos, headingDeg);
			break;
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: CreateSmokeParticle(int ownerPlayerID, Vector2df setPos, Uint32 headingDeg, float setScale) {
	// Find the first smoke particle that is inactive:
	for(Uint32 i = 0; i < MAX_SMOKE_PARTICLES; i++) {
		if(!smokeParticles[i]->IsActive()) {
			smokeParticles[i]->Activate(ownerPlayerID, setPos, headingDeg, setScale);
			break;
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: CreatePickUp(Vector2df setPos, PickUpType setType) {
    // Find the first pick up that is inactive:
	for(Uint32 i = 0; i < MAX_PICK_UPS; i++) {
		if(!pickUps[i]->IsActive()) {
			pickUps[i]->Activate(setPos, setType);
			break;
		}
	}
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
void GameSim ::DrawAllCharacters(Vector2df camPos) {
	for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
		if(playerCharacters[i]->IsActive()) {
			playerCharacters[i]->Draw(camPos);
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim ::DrawAllProjectiles(Vector2df camPos) {
	// draw all bullets:
	for(Uint32 i = 0; i < MAX_BULLETS; i++) {
		if(bullets[i]->IsActive()) {
			bullets[i]->Draw(camPos);
		}
	}

	// draw all missiles:
	for(Uint32 i = 0; i < MAX_MISSILES; i++) {
		if(missiles[i]->IsActive()) {
			missiles[i]->Draw(camPos);
		}
	}

	// draw all mines:
	for(Uint32 i = 0; i < MAX_MINES; i++) {
		if(mines[i]->IsActive()) {
			mines[i]->Draw(camPos);
		}
	}

	// draw all explosion particles:
	for(Uint32 i = 0; i < MAX_EXPLOSION_PARTICLES; i++) {
		if(explosionParticles[i]->IsActive()) {
			explosionParticles[i]->Draw(camPos);
		}
	}

	// draw all smoke particles:
	for(Uint32 i = 0; i < MAX_SMOKE_PARTICLES; i++) {
		if(smokeParticles[i]->IsActive()) {
			smokeParticles[i]->Draw(camPos);
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim ::DrawAllPickUps(Vector2df camPos) {
	for(Uint32 i = 0; i < MAX_PICK_UPS; i++) {
		if(pickUps[i]->IsActive() && pickUps[i]->IsSpawned()) {
			pickUps[i]->Draw(camPos);
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim ::DrawMap(Vector2df camPos) {
	gameMap->Draw(camPos);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: UpdateAllCharacters(float timeDelta) {
	for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
		if(playerCharacters[i]->IsActive()) {
			playerCharacters[i]->Update(timeDelta);
		}
	}

	UpdateRespawn(timeDelta);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: UpdateRespawn(float timeDelta) {
	for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
		if(playerCharacters[i]->IsActive() && playerCharacters[i]->IsDead()) {
			// If the player just died:
			if(!playerCharacters[i]->GetRespawnMe()) {
				playerCharacters[i]->SetRespawnMe(true);
				playerCharacters[i]->SetRespawnTime(SDL_GetTicks() + RESPAWN_DELAY); // FIXTHIS timing?

				// Add point to his killer:
				if(playerCharacters[i]->GetKillerId() < MAX_PLAYERS) {
					playerCharacters[ playerCharacters[i]->GetKillerId() ]->AddPoint();
				}

				//SendKillMessageToAll(players[i].pc->GetKillerId(), i); FIXTHIS
			}
			// If the player is already dead and is ready to respawn:
			else if(SDL_GetTicks() > playerCharacters[i]->GetRespawnTime()) {
				playerCharacters[i]->SetRespawnMe(false);
				SpawnPlayerCharacter(i);
			}
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: UpdateAllProjectiles(float timeDelta) {
	// update all bullets:
	for(Uint32 i = 0; i < MAX_BULLETS; i++) {
		if(bullets[i]->IsActive()) {
			bullets[i]->Update(timeDelta);
		}
	}

	// update all missiles:
	for(Uint32 i = 0; i < MAX_MISSILES; i++) {
		if(missiles[i]->IsActive()) {
			missiles[i]->Update(timeDelta);
		}
	}

	// update all explosion particles:
	for(Uint32 i = 0; i < MAX_EXPLOSION_PARTICLES; i++) {
		if(explosionParticles[i]->IsActive()) {
			explosionParticles[i]->Update(timeDelta);
		}
	}

	// update all smoke particles:
	for(Uint32 i = 0; i < MAX_SMOKE_PARTICLES; i++) {
		if(smokeParticles[i]->IsActive()) {
			smokeParticles[i]->Update(timeDelta);
		}
	}

	// update all mines:
	for(Uint32 i = 0; i < MAX_MINES; i++) {
		if(mines[i]->IsActive()) {
			mines[i]->Update(timeDelta);
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void GameSim :: UpdateAllPickUps(float timeDelta) {
	for(Uint32 i = 0; i < MAX_PICK_UPS; i++) {
		if(pickUps[i]->IsActive()) {
			pickUps[i]->Update(timeDelta);

			// Check if a player collides with the pickUp:
			if(pickUps[i]->IsSpawned()) {
				// For each player:
				for(Uint32 j = 0; j < MAX_PLAYERS; j++) {
					if(playerCharacters[j]->IsActive() && pickUps[i]->IsSolid(playerCharacters[j]->GetPos().x, playerCharacters[j]->GetPos().y)) {
						if(pickUps[i]->GetType() == PICK_UP_TYPE_REPAIR) {
							playerCharacters[j]->AddHealth(PICK_UP_HEALTH_AMOUNT);
						}
						else if(pickUps[i]->GetType() == PICK_UP_TYPE_BULLETS) {
							playerCharacters[j]->AddBullets(PICK_UP_BULLETS_AMOUNT);
						}
						else if(pickUps[i]->GetType() == PICK_UP_TYPE_MISSILES) {
							playerCharacters[j]->AddMissiles(PICK_UP_MISSILES_AMOUNT);
						}
						else if(pickUps[i]->GetType() == PICK_UP_TYPE_MINES) {
							playerCharacters[j]->AddMines(PICK_UP_MINES_AMOUNT);
						}

						pickUps[i]->Despawn();
						break; // Dont need to check if anymore players will collide with this pickup because it was just despawned.
					}
				}
			}
		}
	}
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool GameSim :: CheckCollisionWithChars(Vector2df atPos) const{
	// Collide with players:
	for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
		if(playerCharacters[i]->IsActive() && playerCharacters[i]->IsSolid(atPos)) {
			return true;
		}
	}

	// Collide with missiles:
	for(Uint32 i = 0; i < MAX_MISSILES; i++) {
		if(missiles[i]->IsActive() && missiles[i]->IsSolid(atPos)) {
			return true;
		}
	}

	// Collide with mines:
	for(Uint32 i = 0; i < MAX_MINES; i++) {
		if(mines[i]->IsActive() && mines[i]->IsSolid(atPos)) {
			return true;
		}
	}
	return false;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool GameSim :: CheckCollisionWithLevel(Vector2df atPos) const {
	return gameMap->IsPosSolid(atPos);
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
bool GameSim :: IsPosSolid(Vector2df atPos) const {
	return  CheckCollisionWithLevel(atPos)  || CheckCollisionWithChars(atPos);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool GameSim :: IsPosGravityWell(Vector2df atPos) const {
	return gameMap->IsPosGravityWell(atPos);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Vector2df GameSim :: GetPosGravity(Vector2df atPos) const{
	return gameMap->GetPosGravity(atPos);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Vector2df GameSim :: GetClosestCharPos(Vector2df atPos) const {
	int closestCharID = -1;
	float closestDistance = 100000.0f; // This large number ensures at least one char will be closer

	for(Uint32 i = 0; i < MAX_PLAYERS; i++) {
		if(playerCharacters[i]->IsActive()) {
			float newDistance = sqrt( pow(atPos.x - playerCharacters[i]->GetPos().x, 2) + pow(atPos.y - playerCharacters[i]->GetPos().y, 2));

			if(newDistance < closestDistance) {
				closestDistance = newDistance;
				closestCharID = i;
			}
		}
	}
	if(closestCharID != -1) {
		return playerCharacters[closestCharID]->GetPos();
	}
	else {
		return Vector2df(0,0);
	}
} // ----------------------------------------------------------------------------------------------
