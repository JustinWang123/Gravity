#include "BaseGame.h"
#include "PacketProtocol.h"
#include "PlayerCharacterControlled.h"
#include <assert.h>

using namespace std;

// ------------------------------------------------------------------------------------------------
BaseGame :: BaseGame()
{
    // Init all players:
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        playerCharacters[i] = new PlayerCharacter(i, this);
    }

    // Init all projectiles:
    for(Uint32 i = 0; i < MAX_PROJECTILES; i++)
    {
        projectiles[i] = new ProjectileBase(i, this);
    }

    // init all pick ups:
    for(Uint32 i = 0; i < MAX_PICK_UPS; i++)
    {
        pickUps[i] = new PickUp(i);
    }

    bulletSurface = LoadSurface("Surfaces/Bullet.bmp");
    missileSurface = LoadSurface("Surfaces/Missile.png");
    mineSurface = LoadSurface("Surfaces/Mine.png");
    smokeSurface = LoadSurface("Surfaces/SmokeParticle.png");
    explosionSurface = LoadSurface("Surfaces/ExplosionParticle.png");

    events[BULLET_HIT_CHAR] = new BulletHitCharEvent(this);
    events[CREATE_SMOKE] = new CreateSmokeEvent(this);
    events[STEER_TOWARDS_CLOSEST_CHAR] = new SteerTowardsClosestCharEvent(this);
    events[STOP_MOVING] = new StopMovingEvent(this);
    events[SELF_DESTRUCT] = new SelfDestructEvent(this);
    events[DEACTIVATE] = new DeactivateEvent(this);

    gameMap = new Map(Vector2df(4, 4), this);

    oldTime = SDL_GetTicks();
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
BaseGame :: ~BaseGame()
{
    // delete all player characters:
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        delete playerCharacters[i];
        playerCharacters[i] = 0;
    }

    // delete all projectiles:
    for(Uint32 i = 0; i < MAX_PROJECTILES; i++)
    {
        delete projectiles[i];
        projectiles[i] = 0;
    }

    // delete all pick ups:
    for(Uint32 i = 0; i < MAX_PICK_UPS; i++)
    {
        delete pickUps[i];
        pickUps[i] = 0;
    }

    // delete all events:
    for(Uint32 i = 0; i < NUM_OF_EVENT_TYPES; i++)
    {
        delete events[i];
        events[i] = 0;
    }

    FreeSurface(bulletSurface);
    FreeSurface(missileSurface);
    FreeSurface(mineSurface);
    FreeSurface(smokeSurface);
    FreeSurface(explosionSurface);

    delete gameMap;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: Update()
{
    float timeDelta = (SDL_GetTicks() - oldTime) / 20.0f;
    oldTime = SDL_GetTicks();

    UpdateAllCharacters(timeDelta);
    UpdateAllProjectiles(timeDelta);
    UpdateAllPickUps(timeDelta);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: Draw(Vector2df camPos)
{
    DrawMap(camPos);
    DrawAllCharacters(camPos);
    DrawAllProjectiles(camPos);
    DrawAllPickUps(camPos);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: LoadMap(string fileName)
{
    gameMap->LoadMap(fileName);
} // ------------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
Uint32 BaseGame :: WriteLevelDataToPacket(Uint8 data[])
{
    return gameMap->WriteLevelDataToPacket(data);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: HandleLevelData(UDPpacket* packet)
{
    gameMap->HandleLevelData(packet);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 BaseGame :: WriteGameStateToPacket(Uint8 data[])
{
    Uint32 dataWritePos = PACKET_UPDATE_WORLD_DATA_START;

	// Insert all players:
    Uint32 numOfPlayers = 0;
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        if(playerCharacters[i]->IsActive())
        {
            dataWritePos += playerCharacters[i]->WriteToPacket(dataWritePos, data);
            numOfPlayers++;
        }
    }
    memcpy(&data[PACKET_UPDATE_WORLD_NUM_OF_PLAYERS], &numOfPlayers, 4 );

	// Insert all projectiles:
    Uint32 numOfProjectiles = 0;
    for(Uint32 i = 0; i < MAX_PROJECTILES; i++)
    {
        if(projectiles[i]->IsActive())
        {
            dataWritePos += projectiles[i]->WriteToPacket(dataWritePos, data);
            numOfProjectiles++;
        }
    }
    memcpy(&data[PACKET_UPDATE_WORLD_NUM_OF_PROJECTILES], &numOfProjectiles, 4 );

// Insert all Pick Ups:
    Uint32 numOfPickUps = 0;
    for(Uint32 i = 0; i < MAX_PICK_UPS; i++)
    {
        if(pickUps[i]->IsActive())
        {
            dataWritePos += pickUps[i]->WriteToPacket(dataWritePos, data);
            numOfPickUps++;
        }
    }
    memcpy(&data[PACKET_UPDATE_WORLD_NUM_OF_PICK_UPS], &numOfPickUps, 4 );

    return dataWritePos;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: HandleUpdateWorld(UDPpacket* packet)
{

    // FIXTHIS
    // Get server time stamp and calculate ping:
    //long time;
    ////memcpy(&time, &packet->data[PACKET_UPDATE_WORLD_TIME_STAMP], 4);
    //Uint32 timeDiff = GetTime() - time;
    //serverPingTime = timeDiff * 2;

    // We set everything to inactive and only activate those things we receive a message about:
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        playerCharacters[i]->SetIsActive(false);
    }

    for(Uint32 i = 0; i < MAX_PROJECTILES; i++)
    {
        projectiles[i]->Deactivate();
    }

    // Get number (amount) of each data type:
    Uint32 numOfPlayers;
    Uint32 numOfProjectiles;
    Uint32 numOfPickUps;
    memcpy(&numOfPlayers, &packet->data[PACKET_UPDATE_WORLD_NUM_OF_PLAYERS], 4);
    memcpy(&numOfProjectiles, &packet->data[PACKET_UPDATE_WORLD_NUM_OF_PROJECTILES], 4);
    memcpy(&numOfPickUps, &packet->data[PACKET_UPDATE_WORLD_NUM_OF_PICK_UPS], 4);

    // Get all the data:
    Uint32 id;
    Uint32 dataReadPos = PACKET_UPDATE_WORLD_DATA_START;

    // Read Players:
    for(Uint32 i = 0; i < numOfPlayers; i++)
    {
        memcpy(&id, &packet->data[dataReadPos + PACKET_WRITE_PLAYER_ID], 4);
        dataReadPos += playerCharacters[id]->ReadFromPacket(dataReadPos, packet->data);
    }

    // Read Projectiles:
    for(Uint32 i = 0; i < numOfProjectiles; i++)
    {
        dataReadPos += ReadProjectilePacket(dataReadPos, packet->data);
    }

    // Read Pick Ups:
    for(Uint32 i = 0; i < numOfPickUps; i++)
    {
        memcpy(&id, &packet->data[dataReadPos + PACKET_WRITE_PICK_UP_ID], 4);
        dataReadPos += pickUps[id]->ReadFromPacket(dataReadPos, packet->data);
    }
} // ----------------------------------------------------------------------------------------------





Uint32 BaseGame :: ReadProjectilePacket(Uint32 dataReadPos, Uint8 data[])
{
	Uint32			readId;
	ProjectileType 	readType;
	Vector2df 		readPos;
    Uint32 			readHeadingDeg;
    float			readSpeed;
    Uint32 			readOwnerPlayerId;
    float 			readLife;
    Uint32 			readHealth;
    bool			readIsMoving;

	memcpy(&readId, 			&data[dataReadPos + PACKET_WRITE_PROJECTILE_ID], 		4);
	memcpy(&readType,	 		&data[dataReadPos + PACKET_WRITE_PROJECTILE_TYPE], 		4);
    memcpy(&readPos.x, 			&data[dataReadPos + PACKET_WRITE_PROJECTILE_POSX], 		4);
    memcpy(&readPos.y, 			&data[dataReadPos + PACKET_WRITE_PROJECTILE_POSY], 		4);
    memcpy(&readHeadingDeg, 	&data[dataReadPos + PACKET_WRITE_PROJECTILE_HEADING], 	4);
    memcpy(&readSpeed, 			&data[dataReadPos + PACKET_WRITE_PROJECTILE_SPEED], 	4);
    memcpy(&readLife, 			&data[dataReadPos + PACKET_WRITE_PROJECTILE_LIFE], 		4);
    memcpy(&readHealth, 		&data[dataReadPos + PACKET_WRITE_PROJECTILE_HEALTH], 	4);
    memcpy(&readOwnerPlayerId, 	&data[dataReadPos + PACKET_WRITE_PROJECTILE_PLAYER_ID], 4);
	memcpy(&readIsMoving, 		&data[dataReadPos + PACKET_WRITE_PROJECTILE_ISMOVING], 	1);

	// set the projectiles type:
	if(projectiles[readId]->Type() != readType)
	{
		SetProjectileType(readId, readType);
	}

	// set the projectiles state:
	projectiles[readId]->SetState(readOwnerPlayerId, readPos, Vector2df(readHeadingDeg), readSpeed, readLife, readHealth, readIsMoving);
	projectiles[readId]->Activate();

	return PACKET_WRITE_PROJECTILE_LENGTH;
}





// ------------------------------------------------------------------------------------------------
void BaseGame :: MakeCharacterControllable(Uint32 id)
{
    if(id < MAX_PLAYERS)
    {
        delete playerCharacters[id];
        playerCharacters[id] = new PlayerCharacterControlled(id, this);
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: DamageCharsInCircle(Uint32 attackerId, Uint32 damage, Vector2df centerPos, float radius)
{
    assert(attackerId < MAX_PLAYERS && "BaseGame::DamageCharsInCircle attackerId < MAX_PLAYERS");

    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        if(playerCharacters[i]->IsActive())
        {
            float distance = (centerPos - playerCharacters[i]->GetPos()).Length();
            if(distance  < radius)
            {
                playerCharacters[i]->DecreaseHealth(attackerId, damage);
            }
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: DamageCharsAtPos(Uint32 attackerId, Uint32 damage, Vector2df atPos)
{
    assert(attackerId < MAX_PLAYERS && "BaseGame::DamageCharsAtPos attackerId < MAX_PLAYERS");

    // damage players:
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        if(playerCharacters[i]->IsActive() && playerCharacters[i]->IsSolid(atPos))
        {
            playerCharacters[i]->DecreaseHealth(attackerId, damage);
        }
    }

    // damage projectiles:
    for(Uint32 i = 0; i < MAX_PROJECTILES; i++)
    {
        if(projectiles[i]->IsActive() && projectiles[i]->IsSolid(atPos))
        {
            projectiles[i]->DecreaseHealth(damage);
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: SpawnPlayerCharacter(Uint32 playerID)
{
    Vector2df spawnPos = gameMap->GetRandomSpawnPointPos();
    playerCharacters[playerID]->Spawn(spawnPos);
} // ----------------------------------------------------------------------------------------------


void BaseGame :: CreateProjectile(ProjectileType type, int ownerPlayerId, Vector2df setPos, Vector2df setHeading)
{
	for(Uint32 i = 0; i < MAX_PROJECTILES; i++)
    {
    	if(!projectiles[i]->IsActive())
		{
			SetProjectileType(i, type);

			if(type == PROJECTILE_TYPE_BULLET)
			{
				projectiles[i]->SetState(ownerPlayerId, setPos, setHeading, BULLET_SPEED, BULLET_LIFE, BULLET_HEALTH, true);
			}
			else if(type == PROJECTILE_TYPE_MISSILE)
			{
				projectiles[i]->SetState(ownerPlayerId, setPos, setHeading, MISSILE_SPEED, MISSILE_LIFE, MISSILE_HEALTH, true);
			}
			else if(type == PROJECTILE_TYPE_MINE)
			{
				projectiles[i]->SetState(ownerPlayerId, setPos, setHeading, MINE_SPEED, MINE_LIFE, MINE_HEALTH, true);
			}
			else if(type == PROJECTILE_TYPE_EXPLOSION)
			{
				CreateExplosion(ownerPlayerId, setPos);
			}
			else if(type == PROJECTILE_TYPE_EXPLOSION_PARTICLE)
			{
				projectiles[i]->SetState(ownerPlayerId, setPos, setHeading, EXPLOSION_PARTICLE_SPEED, EXPLOSION_PARTICLE_LIFE, EXPLOSION_PARTICLE_HEALTH, true);
			}
			else if(type == PROJECTILE_TYPE_SMOKE)
			{
				projectiles[i]->SetState(ownerPlayerId, setPos, setHeading, SMOKE_PARTICLE_SPEED, SMOKE_PARTICLE_LIFE, SMOKE_PARTICLE_HEALTH, true);
			}

			projectiles[i]->Activate();

			break;
		}
    }
}

void BaseGame :: SetProjectileType(Uint32 id, ProjectileType setType)
{
	if(setType == PROJECTILE_TYPE_BULLET)
	{
		projectiles[id]->SetType(setType, 1.0f, bulletSurface);
		projectiles[id]->SetHitCharEvent(events[BULLET_HIT_CHAR]);
		projectiles[id]->SetHitSolidEvent(events[BULLET_HIT_CHAR]);
		projectiles[id]->SetOutOfLifeEvent(events[DEACTIVATE]);
		projectiles[id]->SetOutOfHealthEvent(events[DEACTIVATE]);
	}
	else if(setType == PROJECTILE_TYPE_MISSILE)
	{
		projectiles[id]->SetType(setType, 1.0f, missileSurface);
		projectiles[id]->SetHitCharEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetHitSolidEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetOutOfLifeEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetOutOfHealthEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetUpdateEvent(events[CREATE_SMOKE]);
	}
	else if(setType == PROJECTILE_TYPE_MINE)
	{
		projectiles[id]->SetType(setType, 1.0f, mineSurface);
		projectiles[id]->SetHitCharEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetHitSolidEvent(events[STOP_MOVING]);
		projectiles[id]->SetOutOfLifeEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetOutOfHealthEvent(events[SELF_DESTRUCT]);
		projectiles[id]->SetUpdateEvent(events[STEER_TOWARDS_CLOSEST_CHAR]);
	}
	else if(setType == PROJECTILE_TYPE_EXPLOSION)
	{
		// Not really a projectile
	}
	else if(setType == PROJECTILE_TYPE_EXPLOSION_PARTICLE)
	{
		projectiles[id]->SetType(setType, 1.0f, explosionSurface);
		projectiles[id]->SetHitCharEvent(events[BULLET_HIT_CHAR]);
		projectiles[id]->SetHitSolidEvent(events[BULLET_HIT_CHAR]);
		projectiles[id]->SetOutOfLifeEvent(events[DEACTIVATE]);
		projectiles[id]->SetOutOfHealthEvent(events[DEACTIVATE]);
		projectiles[id]->SetUpdateEvent(events[CREATE_SMOKE]);
	}
	else if(setType == PROJECTILE_TYPE_SMOKE)
	{
		projectiles[id]->SetType(setType, 1.0f, smokeSurface);
		//projectiles[id]->SetHitCharEvent(events[DEACTIVATE]);
		//projectiles[id]->SetHitSolidEvent(events[DEACTIVATE]);
		projectiles[id]->SetOutOfLifeEvent(events[DEACTIVATE]);
		projectiles[id]->SetOutOfHealthEvent(events[DEACTIVATE]);
	}
}




// ------------------------------------------------------------------------------------------------
void BaseGame :: CreateExplosion(int ownerPlayerID, Vector2df setPos)
{
    for(Uint32 i = 0; i < 360; i += 35)
    {
        CreateProjectile(PROJECTILE_TYPE_EXPLOSION_PARTICLE, ownerPlayerID, setPos, Vector2df(i));
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: CreatePickUp(Vector2df setPos, PickUpType setType)
{
    // Find the first pick up that is inactive:
    for(Uint32 i = 0; i < MAX_PICK_UPS; i++)
    {
        if(!pickUps[i]->IsActive())
        {
            pickUps[i]->Activate(setPos, setType);
            break;
        }
    }
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
void BaseGame :: DrawAllCharacters(Vector2df camPos)
{
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        if(playerCharacters[i]->IsActive())
        {
            playerCharacters[i]->Draw(camPos);
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame ::DrawAllProjectiles(Vector2df camPos)
{
    // draw all projectiles:
    for(Uint32 i = 0; i < MAX_PROJECTILES; i++)
    {
        if(projectiles[i]->IsActive())
        {
            projectiles[i]->Draw(camPos);
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame ::DrawAllPickUps(Vector2df camPos)
{
    for(Uint32 i = 0; i < MAX_PICK_UPS; i++)
    {
        if(pickUps[i]->IsActive() && pickUps[i]->IsSpawned())
        {
            pickUps[i]->Draw(camPos);
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame ::DrawMap(Vector2df camPos)
{
    gameMap->Draw(camPos);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: UpdateAllCharacters(float timeDelta)
{
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        if(playerCharacters[i]->IsActive())
        {
            playerCharacters[i]->Update(timeDelta);
        }
    }

    UpdateRespawn(timeDelta);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: UpdateRespawn(float timeDelta)
{
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        if(playerCharacters[i]->IsActive() && playerCharacters[i]->IsDead())
        {
            // If the player just died:
            if(!playerCharacters[i]->GetRespawnMe())
            {
                playerCharacters[i]->SetRespawnMe(true);
                playerCharacters[i]->SetRespawnTime(SDL_GetTicks() + RESPAWN_DELAY); // FIXTHIS timing?

                // Add point to his killer:
                if(playerCharacters[i]->GetKillerId() < MAX_PLAYERS)
                {
                    playerCharacters[ playerCharacters[i]->GetKillerId() ]->AddPoint();
                }

                //SendKillMessageToAll(players[i].pc->GetKillerId(), i); FIXTHIS
            }
            // If the player is already dead and is ready to respawn:
            else if(SDL_GetTicks() > playerCharacters[i]->GetRespawnTime())
            {
                playerCharacters[i]->SetRespawnMe(false);
                SpawnPlayerCharacter(i);
            }
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: UpdateAllProjectiles(float timeDelta)
{
    // update all projectiles:
    for(Uint32 i = 0; i < MAX_PROJECTILES; i++)
    {
        if(projectiles[i]->IsActive())
        {
            projectiles[i]->Update(timeDelta);
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void BaseGame :: UpdateAllPickUps(float timeDelta)
{
    for(Uint32 i = 0; i < MAX_PICK_UPS; i++)
    {
        if(pickUps[i]->IsActive())
        {
            pickUps[i]->Update(timeDelta);

            // Check if a player collides with the pickUp:
            if(pickUps[i]->IsSpawned())
            {
                // For each player:
                for(Uint32 j = 0; j < MAX_PLAYERS; j++)
                {
                    if(playerCharacters[j]->IsActive() && pickUps[i]->IsSolid(playerCharacters[j]->GetPos().x, playerCharacters[j]->GetPos().y))
                    {
                        if(pickUps[i]->GetType() == PICK_UP_TYPE_REPAIR)
                        {
                            playerCharacters[j]->AddHealth(PICK_UP_HEALTH_AMOUNT);
                        }
                        else if(pickUps[i]->GetType() == PICK_UP_TYPE_BULLETS)
                        {
                            playerCharacters[j]->AddBullets(PICK_UP_BULLETS_AMOUNT);
                        }
                        else if(pickUps[i]->GetType() == PICK_UP_TYPE_MISSILES)
                        {
                            playerCharacters[j]->AddMissiles(PICK_UP_MISSILES_AMOUNT);
                        }
                        else if(pickUps[i]->GetType() == PICK_UP_TYPE_MINES)
                        {
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
bool BaseGame :: CheckCollisionWithChars(Vector2df atPos) const
{
    // Collide with players:
    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        if(playerCharacters[i]->IsActive() && playerCharacters[i]->IsSolid(atPos))
        {
            return true;
        }
    }

    // Collide with projectiles:
    for(Uint32 i = 0; i < MAX_PROJECTILES; i++)
    {
        if(projectiles[i]->IsActive() && projectiles[i]->IsSolid(atPos))
        {
            return true;
        }
    }
    return false;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool BaseGame :: CheckCollisionWithLevel(Vector2df atPos) const
{
    assert(gameMap && "BaseGame :: CheckCollisionWithLevel gameMap");
    return gameMap->IsPosSolid(atPos);
} // ----------------------------------------------------------------------------------------------





// ------------------------------------------------------------------------------------------------
bool BaseGame :: IsPosSolid(Vector2df atPos) const
{
    return  CheckCollisionWithLevel(atPos)  || CheckCollisionWithChars(atPos);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool BaseGame :: IsPosGravityWell(Vector2df atPos) const
{
    return gameMap->IsPosGravityWell(atPos);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Vector2df BaseGame :: GetPosGravity(Vector2df atPos) const
{
    return gameMap->GetPosGravity(atPos);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Vector2df BaseGame :: GetClosestCharPos(Vector2df atPos) const
{
    int closestCharID = -1;
    float closestDistance = 100000.0f; // This large number ensures at least one char will be closer

    for(Uint32 i = 0; i < MAX_PLAYERS; i++)
    {
        if(playerCharacters[i]->IsActive())
        {
            float newDistance = sqrt( pow(atPos.x - playerCharacters[i]->GetPos().x, 2) + pow(atPos.y - playerCharacters[i]->GetPos().y, 2));

            if(newDistance < closestDistance)
            {
                closestDistance = newDistance;
                closestCharID = i;
            }
        }
    }
    if(closestCharID != -1)
    {
        return playerCharacters[closestCharID]->GetPos();
    }
    else
    {
        return Vector2df(0,0);
    }
} // ----------------------------------------------------------------------------------------------
