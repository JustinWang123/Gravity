#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#include "SDLWrapper.h"
#include "Vector2df.h"
class BaseGame;

class EntityBase
{
public:
    EntityBase(Uint32 setId, BaseGame* setGame);

    virtual void 			Update(float timeDelta)=0;
    virtual void			Draw(Vector2df camPos) const=0;

    // Accessors:
    bool					IsActive() const;
    Uint32 					Id() const;
    Vector2df				Pos() const;
    BaseGame*				Game() const;

    // Modifiers:
    void					Activate();
    void					Deactivate();
    Vector2df				SetPos(Vector2df setPos);

private:
    Uint32 					id;

    bool					isActive;

    Vector2df				pos;

    BaseGame* 				game;
};

#endif
