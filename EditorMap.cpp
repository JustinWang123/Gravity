#include "EditorMap.h"
#include <fstream>

EditorMap :: EditorMap(Vector2df setScreenPos, BaseGame* setBaseGame)
    : Map(setScreenPos, setBaseGame)
{
}

EditorMap :: ~EditorMap()
{
}

void EditorMap :: SetTile(Uint32 tileX, Uint32 tileY, TileType type)
{
    tileTypes[tileX][tileY] = type;
}

SDL_Surface*  EditorMap :: GetTileSurface(TileType type)
{
    return tileSurfaces[(int)type];
}

void EditorMap :: SaveMap()
{
    std::ofstream myfile;
    myfile.open ("Data/newLevel.txt", std::ios::trunc);

    for(Uint32 y = 0; y < MAP_NUM_TILES_Y; y++)
        for(Uint32 x = 0; x < MAP_NUM_TILES_X; x++)
        {
            myfile << (int)tileTypes[x][y];
            myfile << " ";
        }

    myfile.close();
}
