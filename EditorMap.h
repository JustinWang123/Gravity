#ifndef EDITOR_MAP_H
#define EDITOR_MAP_H

#include "Map.h"

class EditorMap : public Map
{
public:
    EditorMap(Vector2df setScreenPos, BaseGame* setBaseGame);
    ~EditorMap();

    void 			SetTile(Uint32 tileX, Uint32 tileY, TileType type);
    SDL_Surface* 	GetTileSurface(TileType type);
    void			SaveMap();

private:
};
#endif // EDITOR_MAP_H
