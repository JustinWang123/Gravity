#ifndef EDITOR_GAME_SIM_H
#define EDITOR_GAME_SIM_H

#include "BaseGame.h"
#include "EditorMap.h"

class EditorGame : public BaseGame
{
public:
    EditorGame();
    virtual ~EditorGame();

    virtual void			Update();
    virtual void			Draw(Vector2df camPos);

    EditorMap*				GetMap()
    {
        return (EditorMap*)gameMap;
    }

};
#endif // EDITOR_GAME_SIM_H
