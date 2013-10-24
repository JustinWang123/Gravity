#include "EditorGame.h"


EditorGame :: EditorGame()
{
    gameMap = new EditorMap(Vector2df(4, 4), this);
}

EditorGame :: ~EditorGame()
{
    delete gameMap;
}

void EditorGame :: Update()
{
}


void EditorGame ::Draw(Vector2df camPos)
{
    DrawMap(camPos);
}
