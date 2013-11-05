#ifndef MAP_H
#define MAP_H

#include "SDLWrapper.h"
#include "Vector2df.h"
class GameBase;

const Uint32 				MAP_TILE_SIZE = 32;
const Uint32 				MAP_NUM_TILES_X = 30;
const Uint32 				MAP_NUM_TILES_Y = 30;
const Uint32				MAP_NUM_TILES_ON_SCREEN_X = 19;
const Uint32				MAP_NUM_TILES_ON_SCREEN_Y = 20;
const float 				MAP_MIN_POS_X = 0.0f;
const float 				MAP_MIN_POS_Y = 0.0f;
const float 				MAP_MAX_POS_X = MAP_NUM_TILES_X * MAP_TILE_SIZE;
const float 				MAP_MAX_POS_Y = MAP_NUM_TILES_Y * MAP_TILE_SIZE;
const float					MAP_GRAVITY_FORCE_SCALER = 0.1f;
const float					MAP_GRAVITY_WELL_FORCE = 0.5f;
const Uint32				MAP_MAX_NUM_OF_SPAWN_POINTS = 10;

typedef Uint32 				Tile;
typedef float 				Pos;

enum TileType
{
    TILE_TYPE_SPACE = 0,
    TILE_TYPE_WALL,
    TILE_TYPE_UP_WELL,
    TILE_TYPE_DOWN_WELL,
    TILE_TYPE_LEFT_WELL,
    TILE_TYPE_RIGHT_WELL,
    TILE_TYPE_BULLET_PICKUP,
    TILE_TYPE_MINE_PICKUP,
    TILE_TYPE_MISSILE_PICKUP,
    TILE_TYPE_REPAIR_PICKUP,
    TILE_TYPE_SPAWN,
    NUM_OF_TILE_TYPES
};


class Map
{
public:
    Map(Vector2df setScreenPos, GameBase* setGameBase);
    ~Map();

    void					LoadMap(std::string fileName);
    void					SaveMap();
    void					CalculateGravityVectors();

    void 					Draw(Vector2df camPos) const;

    Uint32 					WriteLevelDataToPacket(Uint8 data[]);
    void					HandleLevelData(UDPpacket* packet);

	// Modifiers:
	void 			SetTile(Uint32 tileX, Uint32 tileY, TileType type);

    // Accessors:
    bool					IsPosSolid(Vector2df atPos) const;
    bool					IsPosGravityWell(Vector2df atPos) const;
    Vector2df				GetPosGravity(Vector2df atPos) const;
    Vector2df				GetRandomSpawnPointPos() const;
    SDL_Surface* 			GetTileSurface(TileType type);


protected:
    void					CreateSpawnPoint(Vector2df atPos);
    bool					IsTileSolid(Tile atTileX, Tile atTileY) const;
    bool					IsTileGravityWell(Tile atTileX, Tile atTileY) const;
    Vector2df				GetTileGravity(Tile atTileX, Tile atTileY) const;

    Vector2df				screenPos;
    TileType 				tileTypes[MAP_NUM_TILES_X][MAP_NUM_TILES_Y];
    SDL_Surface* 			tileSurfaces[NUM_OF_TILE_TYPES];
    Vector2df				tileGravity[MAP_NUM_TILES_X][MAP_NUM_TILES_Y];
    Vector2df				spawnPoints[MAP_MAX_NUM_OF_SPAWN_POINTS];

    GameBase* 				game;
    Uint32					currentSpawnPoint;
};

#endif
