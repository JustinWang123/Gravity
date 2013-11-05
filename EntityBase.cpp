#include "EntityBase.h"
#include "Map.h"
#include <assert.h>

EntityBase :: EntityBase(Uint32 setId, GameBase* setGame)
    :	id(setId),
    	isActive(false),
		game(setGame)
{
}

Uint32 EntityBase :: Id() const
{
    return id;
}

bool EntityBase :: IsActive() const
{
    return isActive;
}

Vector2df EntityBase :: Pos() const
{
    return pos;
}

GameBase* EntityBase :: Game() const
{
	return game;
}

void EntityBase :: Activate()
{
    isActive = true;
}

void EntityBase :: Deactivate()
{
    isActive = false;
}

void EntityBase :: SetPos(Vector2df setPos)
{
    pos = setPos;
}
