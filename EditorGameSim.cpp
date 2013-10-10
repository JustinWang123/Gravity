#include "EditorGameSim.h"


EditorGameSim :: EditorGameSim() {
	gameMap = new EditorMap(Vector2df(4, 4), this);
}

EditorGameSim :: ~EditorGameSim() {
	delete gameMap;
}

void EditorGameSim :: Update() {
}


void EditorGameSim ::Draw(Vector2df camPos) {
 	DrawMap(camPos);
}
