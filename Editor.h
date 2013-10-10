#ifndef EDITOR_H
#define EDITOR_H

#include "Game.h"
#include "Button.h"
#include "Vector2df.h"
class EditorMap;

const float EDITOR_CAM_SPEED =  5.0f;

class Editor : public Game {
public:
	Editor();
	~Editor();

	virtual 				void Update();
	virtual 				void Draw();

	bool 					GetExit() const {return exit;}


private:
	void					DrawTileSelector();

	Button					exitButton;
	Button					saveButton;
	SDL_Surface*			inGameMenu;

	Vector2df camPos;

	EditorMap*				editorMap;
	int 					currentTile;
	SDL_Surface*			selector;
};

#endif // EDITOR_H
