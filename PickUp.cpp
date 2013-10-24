#include "PickUp.h"
#include "PacketProtocol.h"
#include <assert.h>
#include "BaseGame.h"

SDL_Surface* PickUp :: repairPickUpSurface = 0;
SDL_Surface* PickUp :: bulletsPickUpSurface = 0;
SDL_Surface* PickUp :: missilesPickUpSurface = 0;
SDL_Surface* PickUp :: minesPickUpSurface = 0;




// ------------------------------------------------------------------------------------------------
void PickUp :: Init()
{
    repairPickUpSurface = LoadSurface("Surfaces/RepairPickUp.png");
    bulletsPickUpSurface = LoadSurface("Surfaces/BulletPickUp.png");
    missilesPickUpSurface = LoadSurface("Surfaces/MissilePickUp.png");
    minesPickUpSurface = LoadSurface("Surfaces/MinePickUp.png");

    assert(repairPickUpSurface && 	"PickUp::Init repairPickUpSurface");
    assert(bulletsPickUpSurface && 	"PickUp::Init bulletsPickUpSurface");
    assert(missilesPickUpSurface && "PickUp::Init missilesPickUpSurface");
    assert(minesPickUpSurface && 	"PickUp::Init minesPickUpSurface");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PickUp :: Destroy()
{
    FreeSurface(repairPickUpSurface);
    FreeSurface(bulletsPickUpSurface);
    FreeSurface(missilesPickUpSurface);
    FreeSurface(minesPickUpSurface);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
PickUp :: PickUp(Uint32 setId)
    :	id(setId),
        type(PICK_UP_TYPE_REPAIR),
        isActive(false),
        isSpawned(false)
{

    assert(id < MAX_PICK_UPS && "PickUp::PickUp id < MAX_PICK_UPS");
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
PickUp :: ~PickUp()
{
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PickUp :: Activate(Vector2df setPos, PickUpType setType)
{
    // Pre-Conditions:
    assert(setPos.x >= MAP_MIN_POS_X && setPos.x < MAP_MAX_POS_X && "PickUp::Activate setPos.x >= MAP_MIN_POS_X && setPos.x < MAP_MAX_POS_X");
    assert(setPos.y >= MAP_MIN_POS_Y && setPos.y < MAP_MAX_POS_Y && "PickUp::Activate setPos.y >= MAP_MIN_POS_Y && setPos.y < MAP_MAX_POS_Y");

    pos = setPos;
    type = setType;
    isActive = true;
    isSpawned = true;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PickUp :: Update(float timeDelta)
{
    // Pre-Conditions:
    assert(isActive && "PickUp::Update isActive");

    if(isActive)
    {
        if(!isSpawned)
        {
            if(timeUntilRespawn <= 0)
                isSpawned = true;
            else
            {
                timeUntilRespawn -= timeDelta;
            }
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PickUp :: Despawn()
{
    isSpawned = false;
    timeUntilRespawn = RESPAWN_TIME;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void PickUp :: Draw(Vector2df camPos) const
{
    // Pre-conditions:
    assert(isActive &&				"PickUp::Draw isActive");
    assert(isSpawned && 			"PickUp::Draw IsSpawned");
    assert(repairPickUpSurface && 	"PickUp::Draw repairPickUpSurface");
    assert(bulletsPickUpSurface && 	"PickUp::Draw bulletsPickUpSurface");
    assert(missilesPickUpSurface && "PickUp::Draw missilesPickUpSurface");
    assert(minesPickUpSurface && 	"PickUp::Draw minesPickUpSurface");

    if(IsActive() && IsSpawned())
    {
        Vector2df screenPos = pos - camPos;

        if(type == PICK_UP_TYPE_REPAIR)
        {
            DrawSurface(screenPos.x - PICK_UP_WIDTH / 2, screenPos.y - PICK_UP_HEIGHT / 2, repairPickUpSurface);
        }
        else if(type == PICK_UP_TYPE_BULLETS)
        {
            DrawSurface(screenPos.x - PICK_UP_WIDTH / 2, screenPos.y - PICK_UP_HEIGHT / 2, bulletsPickUpSurface);
        }
        else if(type == PICK_UP_TYPE_MISSILES)
        {
            DrawSurface(screenPos.x - PICK_UP_WIDTH / 2, screenPos.y - PICK_UP_HEIGHT / 2, missilesPickUpSurface);
        }
        else if(type == PICK_UP_TYPE_MINES)
        {
            DrawSurface(screenPos.x - PICK_UP_WIDTH / 2, screenPos.y - PICK_UP_HEIGHT / 2, minesPickUpSurface);
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool PickUp :: IsSolid(Pos atPosX, Pos atPosY) const
{
    assert(isActive && 												"PickUp::IsSolid isActive");
    assert(isSpawned && 											"PickUp::IsSolid isSpawned");
    assert(atPosX >= MAP_MIN_POS_X && atPosX < MAP_MAX_POS_X && 	"PickUp::IsSolid atPos.x >= MAP_MIN_POS_X && atPos.x < MAP_MAX_POS_X");
    assert(atPosY >= MAP_MIN_POS_Y && atPosY < MAP_MAX_POS_Y && 	"PickUp::IsSolid atPos.y >= MAP_MIN_POS_Y && atPos.y < MAP_MAX_POS_Y");

    return 	IsActive()
            &&      IsSpawned()
            && 		atPosX > (Uint32)pos.x - PICK_UP_WIDTH / 2
            &&		atPosX < (Uint32)pos.x + PICK_UP_WIDTH / 2
            &&		atPosY > (Uint32)pos.y - PICK_UP_HEIGHT / 2
            &&		atPosY < (Uint32)pos.y + PICK_UP_HEIGHT / 2;
} // ----------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------
Uint32 PickUp :: WriteToPacket(Uint32 dataWritePos, Uint8 data[])
{
    // Pre-Conditions:
    assert(isActive && 											"PickUp::WriteToPacket isActive");
    assert(pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X && 	"PickUp::WriteToPacket pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
    assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && 	"PickUp::WriteToPacket pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");

    memcpy(&data[dataWritePos + PACKET_WRITE_PICK_UP_ID], &id, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PICK_UP_IS_SPAWNED], &isSpawned, 1);
    memcpy(&data[dataWritePos + PACKET_WRITE_PICK_UP_RESPAWN_TIME], &timeUntilRespawn, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PICK_UP_POSX], &pos.x, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PICK_UP_POSY], &pos.y, 4);
    memcpy(&data[dataWritePos + PACKET_WRITE_PICK_UP_TYPE], &type, 4);
    return PACKET_WRITE_PICK_UP_LENGTH;
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Uint32 PickUp :: ReadFromPacket(Uint32 dataReadPos, Uint8 data[])
{
    memcpy(&isSpawned, &data[dataReadPos + PACKET_WRITE_PICK_UP_IS_SPAWNED], 1);
    memcpy(&timeUntilRespawn, &data[dataReadPos + PACKET_WRITE_PICK_UP_RESPAWN_TIME], 4);
    memcpy(&pos.x, &data[dataReadPos + PACKET_WRITE_PICK_UP_POSX], 4);
    memcpy(&pos.y, &data[dataReadPos + PACKET_WRITE_PICK_UP_POSY], 4);
    memcpy(&type, &data[dataReadPos + PACKET_WRITE_PICK_UP_TYPE], 4);

    isActive = true;

    // Post-Conditions:
    assert(isActive && 											"PickUp::ReadFromPacket isActive");
    assert(pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X && 	"PickUp::ReadFromPacket pos.x >= MAP_MIN_POS_X && pos.x < MAP_MAX_POS_X");
    assert(pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y && 	"PickUp::ReadFromPacket pos.y >= MAP_MIN_POS_Y && pos.y < MAP_MAX_POS_Y");

    return PACKET_WRITE_PICK_UP_LENGTH;
} // ----------------------------------------------------------------------------------------------
