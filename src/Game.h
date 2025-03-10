#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> // математика

#include "gameLevel.h"
#include "BallObject.h"

enum GameState{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

bool CheckCollision(GameObject& one, GameObject& two); //AABB - AABB collision
bool CheckCollision(BallObject& ball, GameObject& two); // Circle -AABB collision
//float clamp(float value, float min, float max);

class Game{
public:
	//game state
	GameState State;
	bool Keys[1024];
	unsigned int Width, Height;
	//Constructor/Destructor
	Game(unsigned int width, unsigned int height);
	~Game();
	//initialize game state (load all shaders / textures/ level)
	void Init();
	//game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();

	std::vector<GameLevel> Levels;
	unsigned int Level;
	void DoCollisions();
	

};

#endif // !GAME_H
