#ifndef EDITOR_GAME_SIM_H
#define EDITOR_GAME_SIM_H

#include "GameSim.h"
#include "EditorMap.h"

class EditorGameSim : public GameSim {
public:
	EditorGameSim();
	virtual ~EditorGameSim();

	virtual void			Update();
	virtual void			Draw(Vector2df camPos);

	EditorMap*				GetMap() {return (EditorMap*)gameMap;}

};
#endif // EDITOR_GAME_SIM_H
