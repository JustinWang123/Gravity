#include "EntityBase.h"
#include "Map.h"
#include <assert.h>

EntityBase :: EntityBase(Uint32 setId, BaseGame* setGame)
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

BaseGame* EntityBase :: Game() const
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

Vector2df EntityBase :: SetPos(Vector2df setPos)
{
    assert(setPos.x >= MAP_MIN_POS_X 	&& "EntityBase::SetPos");
    assert(setPos.x < MAP_MAX_POS_X 	&& "EntityBase::SetPos");
    assert(setPos.y >= MAP_MIN_POS_Y 	&& "EntityBase::SetPos");
    assert(setPos.y < MAP_MAX_POS_Y 	&& "EntityBase::SetPos");

    pos = setPos;
}
