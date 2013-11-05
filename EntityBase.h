#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#include "SDLWrapper.h"
#include "Vector2df.h"
class GameBase;

class EntityBase
{
public:
    EntityBase(Uint32 setId, GameBase* setGame);

    virtual void 			Update(float timeDelta)=0;
    virtual void			Draw(Vector2df camPos) const=0;

    // Accessors:
    bool					IsActive() const;
    Uint32 					Id() const;
    Vector2df				Pos() const;
    GameBase*				Game() const;

    // Modifiers:
    void					Activate();
    void					Deactivate();
    void					SetPos(Vector2df setPos);

private:
    Uint32 					id;

    bool					isActive;

    Vector2df				pos;

    GameBase* 				game;
};

#endif
