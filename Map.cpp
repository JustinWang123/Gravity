#include "Map.h"
#include <fstream>
#include <assert.h>
#include <math.h>
#include "Game.h"

// ------------------------------------------------------------------------------------------------
Map :: Map(Vector2df setScreenPos, BaseGame* setBaseGame)
{
    tileSurfaces[TILE_TYPE_SPACE] = LoadSurface("Surfaces/Space.png");
    tileSurfaces[TILE_TYPE_WALL] = LoadSurface("Surfaces/Metal_Wall_01.png");
    tileSurfaces[TILE_TYPE_UP_WELL] = LoadSurface("Surfaces/UpGravityWell.png");
    tileSurfaces[TILE_TYPE_DOWN_WELL] = LoadSurface("Surfaces/DownGravityWell.png");
    tileSurfaces[TILE_TYPE_LEFT_WELL] = LoadSurface("Surfaces/LeftGravityWell.png");
    tileSurfaces[TILE_TYPE_RIGHT_WELL] = LoadSurface("Surfaces/RightGravityWell.png");
    tileSurfaces[TILE_TYPE_BULLET_PICKUP] = LoadSurface("Surfaces/BulletPickUpTile.png");
    tileSurfaces[TILE_TYPE_MINE_PICKUP] = LoadSurface("Surfaces/MinePickupTile.png");
    tileSurfaces[TILE_TYPE_MISSILE_PICKUP] = LoadSurface("Surfaces/MissilePickUpTile.png");
    tileSurfaces[TILE_TYPE_REPAIR_PICKUP] = LoadSurface("Surfaces/RepairPickUpTile.png");
    tileSurfaces[TILE_TYPE_SPAWN] = LoadSurface("Surfaces/SpawnTile.png");

    screenPos = setScreenPos;
    game = setBaseGame;
    currentSpawnPoint = 0;

    for(Tile y = 0; y < MAP_NUM_TILES_Y; y++)
    {
        for(Tile x = 0; x < MAP_NUM_TILES_X; x++)
        {
            tileTypes[x][y] = TILE_TYPE_SPACE;
        }
    }

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Map :: ~Map()
{
    for(int i = 0; i < NUM_OF_TILE_TYPES; i++)
    {
        if(tileSurfaces[i] != 0)
        {
            FreeSurface(tileSurfaces[i]);
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Map :: LoadMap(std::string fileName)
{
    std::ifstream mapFile (fileName.c_str());
    assert(mapFile.is_open());

    int data;
    for(Tile y = 0; y < MAP_NUM_TILES_Y; y++)
    {
        for(Tile x = 0; x < MAP_NUM_TILES_X; x++)
        {
            mapFile >> data;

            if( (TileType)data < TILE_TYPE_BULLET_PICKUP)
            {
                tileTypes[x][y] = (TileType)data;
            }
            else
            {
                tileTypes[x][y] = TILE_TYPE_SPACE;

                if((TileType)data == TILE_TYPE_BULLET_PICKUP)
                {
                    game->CreatePickUp(Vector2df(x * MAP_TILE_SIZE + MAP_TILE_SIZE / 2, y * MAP_TILE_SIZE + MAP_TILE_SIZE / 2), PICK_UP_TYPE_BULLETS);
                }
                else if((TileType)data == TILE_TYPE_MINE_PICKUP)
                {
                    game->CreatePickUp(Vector2df(x * MAP_TILE_SIZE + MAP_TILE_SIZE / 2, y * MAP_TILE_SIZE + MAP_TILE_SIZE / 2), PICK_UP_TYPE_MINES);
                }
                else if((TileType)data == TILE_TYPE_MISSILE_PICKUP)
                {
                    game->CreatePickUp(Vector2df(x * MAP_TILE_SIZE + MAP_TILE_SIZE / 2, y * MAP_TILE_SIZE + MAP_TILE_SIZE / 2), PICK_UP_TYPE_MISSILES);
                }
                else if((TileType)data == TILE_TYPE_REPAIR_PICKUP)
                {
                    game->CreatePickUp(Vector2df(x * MAP_TILE_SIZE + MAP_TILE_SIZE / 2, y * MAP_TILE_SIZE + MAP_TILE_SIZE / 2), PICK_UP_TYPE_REPAIR);
                }
                else if((TileType)data == TILE_TYPE_SPAWN)
                {
                    CreateSpawnPoint(Vector2df(x * MAP_TILE_SIZE + MAP_TILE_SIZE / 2, y * MAP_TILE_SIZE + MAP_TILE_SIZE / 2));
                }
            }
        }
    }

    mapFile.close();

    CalculateGravityVectors();
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Map :: CalculateGravityVectors()
{
    Vector2df closestTile[MAP_NUM_TILES_X][MAP_NUM_TILES_Y];

    // initilize all closestTiles to be very far away so that we are guranteed to get a closer tile from map
    for(Tile y = 0; y < MAP_NUM_TILES_Y; y++)
    {
        for(Tile x = 0; x < MAP_NUM_TILES_X; x++)
        {
            closestTile[x][y].Set(MAP_NUM_TILES_X * 2, MAP_NUM_TILES_Y * 2);
        }
    }

    // For every tile
    for(Tile y = 0; y < MAP_NUM_TILES_Y; y++)
    {
        for(Tile x = 0; x < MAP_NUM_TILES_X; x++)
        {
            // Check every other tile
            for(Tile testY = 0; testY < MAP_NUM_TILES_Y; testY++)
            {
                for(Tile testX = 0; testX < MAP_NUM_TILES_X; testX++)
                {
                    // If the tile is a solid tile and if it is closer then the previous 'closestTile'
                    if((x != testX || y != testY) && IsTileSolid(testX, testY))
                    {
                        float distanceToCurrentTile = sqrt( pow((Sint32)x - closestTile[x][y].x, 2) + pow((Sint32)y - closestTile[x][y].y, 2));
                        float distanceToTestTile = sqrt( pow((Sint32)x - (Sint32)testX, 2) + pow((Sint32)y - (Sint32)testY, 2));

                        if(distanceToTestTile < distanceToCurrentTile)
                        {
                            // then set the tile to be the new closest tile
                            closestTile[x][y].Set(testX, testY);
                        }
                    }
                }
            }
        }
    }

    // The gravity vector of each tile is the normal vector pointing towards the closest tile:
    for(Tile x = 0; x < MAP_NUM_TILES_X; x++)
    {
        for(Tile y = 0; y < MAP_NUM_TILES_Y; y++)
        {
            if(tileTypes[x][y] == TILE_TYPE_UP_WELL)
            {
                tileGravity[x][y].Set(0, -MAP_GRAVITY_WELL_FORCE);
            }
            else if(tileTypes[x][y] == TILE_TYPE_DOWN_WELL)
            {
                tileGravity[x][y].Set(0, MAP_GRAVITY_WELL_FORCE);
            }
            else if(tileTypes[x][y] == TILE_TYPE_LEFT_WELL)
            {
                tileGravity[x][y].Set(-MAP_GRAVITY_WELL_FORCE, 0);
            }
            else if(tileTypes[x][y] == TILE_TYPE_RIGHT_WELL)
            {
                tileGravity[x][y].Set(MAP_GRAVITY_WELL_FORCE, 0);
            }
            else
            {
                tileGravity[x][y].Set(closestTile[x][y].x - x, closestTile[x][y].y - y);
                tileGravity[x][y] *= MAP_GRAVITY_FORCE_SCALER;
            }
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Map :: Draw(Vector2df camPos) const
{

    int camTileX = camPos.x / MAP_TILE_SIZE; // the map tile at camPos.x
    int camTileY = camPos.y / MAP_TILE_SIZE; // the map tile at camPos.y
    int camTileOffsetX = ((int)camPos.x - camTileX * MAP_TILE_SIZE); // the offset of the camera
    int camTileOffsetY = ((int)camPos.y - camTileY * MAP_TILE_SIZE); // the offset of the camera

    // for each of the screen tiles
    for(Tile screenTileX = 0; screenTileX < MAP_NUM_TILES_ON_SCREEN_X; screenTileX++)
    {
        for(Tile screenTileY = 0; screenTileY < MAP_NUM_TILES_ON_SCREEN_Y; screenTileY++)
        {
            // determine which map tile is going to be drawn for this screen tile
            Uint32 mapTileX = camTileX + screenTileX;
            Uint32 mapTileY = camTileY + screenTileY;

            // only draw the screen tile if the map tile is actually a valid tile (within the bounds of the map)
            if(mapTileX >= 0 && mapTileX < MAP_NUM_TILES_X && mapTileY >= 0 && mapTileY < MAP_NUM_TILES_Y)
            {
                DrawSurface(
                    screenPos.x - camTileOffsetX + screenTileX * MAP_TILE_SIZE, // ScreenPosX of tile
                    screenPos.y - camTileOffsetY + screenTileY * MAP_TILE_SIZE, // ScreenPosY of tile
                    tileSurfaces[tileTypes[mapTileX][mapTileY]]); // TileType
            }
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool Map :: IsPosSolid(Vector2df atPos) const
{
    // Out of bounds positions are always considered solid:
    if(atPos.x < MAP_MIN_POS_X  || atPos.x >= MAP_MAX_POS_X || atPos.y < MAP_MIN_POS_Y || atPos.y >= MAP_MAX_POS_Y)
    {
        return true;
    }
    else
    {
        int tileX = (atPos.x - screenPos.x) / MAP_TILE_SIZE;
        int tileY = (atPos.y - screenPos.y) / MAP_TILE_SIZE;

        return IsTileSolid(tileX, tileY);
    }
} // ----------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
bool Map :: IsTileSolid(Tile atTileX, Tile atTileY) const
{
    // Out of bounds tiles are always considered solid:
    if(atTileX < 0 || atTileX >= MAP_NUM_TILES_X || atTileY < 0 || atTileY >= MAP_NUM_TILES_Y)
    {
        return true;
    }
    else
    {
        return tileTypes[atTileX][atTileY] == TILE_TYPE_WALL;
    }
} // --------------------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------------------
bool Map :: IsPosGravityWell(Vector2df atPos) const
{
    // Out of bounds positions are always considered not gravity wells:
    if(atPos.x < MAP_MIN_POS_X  || atPos.x >= MAP_MAX_POS_X || atPos.y < MAP_MIN_POS_Y || atPos.y >= MAP_MAX_POS_Y)
    {
        return false;
    }
    else
    {
        int tileX = (atPos.x - screenPos.x) / MAP_TILE_SIZE;
        int tileY = (atPos.y - screenPos.y) / MAP_TILE_SIZE;

        return IsTileGravityWell(tileX, tileY);
    }
} // ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
bool Map :: IsTileGravityWell(Tile atTileX, Tile atTileY) const
{
    // Out of bounds tiles are always considered not to be gravity wells:
    if(atTileX < 0 || atTileX >= MAP_NUM_TILES_X || atTileY < 0 || atTileY >= MAP_NUM_TILES_Y)
    {
        return false;
    }
    else
    {
        return tileTypes[atTileX][atTileY] == TILE_TYPE_DOWN_WELL
               || tileTypes[atTileX][atTileY] == TILE_TYPE_UP_WELL
               || tileTypes[atTileX][atTileY] == TILE_TYPE_RIGHT_WELL
               || tileTypes[atTileX][atTileY] == TILE_TYPE_LEFT_WELL;
    }
} // --------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
Vector2df Map :: GetPosGravity(Vector2df atPos) const
{
    // Out of bounds positions are always considered to have 0 gravity:
    if(atPos.x < MAP_MIN_POS_X  || atPos.x >= MAP_MAX_POS_X || atPos.y < MAP_MIN_POS_Y || atPos.y >= MAP_MAX_POS_Y)
    {
        return Vector2df();
    }
    else
    {
        // This is probly fucky:
        int tileX = (atPos.x - screenPos.x) / MAP_TILE_SIZE;
        int tileY = (atPos.y - screenPos.y) / MAP_TILE_SIZE;

        return GetTileGravity(tileX, tileY);
    }
} // --------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
Vector2df Map :: GetTileGravity(Tile atTileX, Tile atTileY) const
{
    // Out of bounds tiles are always considered to have gravity of 0:
    if(atTileX < 0 || atTileX >= MAP_NUM_TILES_X || atTileY < 0 || atTileY >= MAP_NUM_TILES_Y)
    {
        return 0.0f;
    }
    else
    {
        return tileGravity[atTileX][atTileY];
    }
} // --------------------------------------------------------------------------------------------





// ----------------------------------------------------------------------------------------------
Vector2df Map :: GetRandomSpawnPointPos() const
{
    if(currentSpawnPoint != 0)
    {
        int spawnPointID = rand() % currentSpawnPoint;
        return spawnPoints[spawnPointID];
    }
    else
    {
        return Vector2df(0,0);
    }
} // --------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
void Map :: CreateSpawnPoint(Vector2df atPos)
{
    assert(currentSpawnPoint < MAP_MAX_NUM_OF_SPAWN_POINTS && "Map::CreateSpawnPoint currentSpawnPoint < MAP_MAX_NUM_OF_SPAWN_POINTS");

    if(currentSpawnPoint < MAP_MAX_NUM_OF_SPAWN_POINTS)
    {
        spawnPoints[currentSpawnPoint] = atPos;
        currentSpawnPoint++;
    }
} // --------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
Uint32 Map :: WriteLevelDataToPacket(Uint8 data[])
{
    Uint32 dataWritePos = PACKET_LEVEL_DATA_CONTENTS;

    for(Uint32 y = 0; y < MAP_NUM_TILES_Y; y++)
    {
        for(Uint32 x = 0; x < MAP_NUM_TILES_X; x++)
        {
            Uint8 temp = (Uint8)tileTypes[x][y];

            memcpy(&data[dataWritePos], &temp, 1 );
            dataWritePos += 1;
        }
    }

    return dataWritePos;
} // --------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
void Map :: HandleLevelData(UDPpacket* packet)
{
    Uint32 dataReadPos = PACKET_LEVEL_DATA_CONTENTS;

    for(Uint32 y = 0; y < MAP_NUM_TILES_Y; y++)
    {
        for(Uint32 x = 0; x < MAP_NUM_TILES_X; x++)
        {
            Uint8 temp;
            memcpy(&temp, &packet->data[dataReadPos], 1 );
            tileTypes[x][y] = (TileType)temp;
            dataReadPos += 1;
        }
    }
} // --------------------------------------------------------------------------------------------
