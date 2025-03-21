#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> // математика
#include "gameLevel.h"
#include "BallObject.h"
#include "powerUp.hpp"
#include <tuple>



// Represents the current state of the game
enum GameState{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};
// Represents the four possible (collision) directions
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};
// Defines a Collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>


bool CheckCollision(GameObject& one, GameObject& two); //AABB - AABB collision
Collision CheckCollision(BallObject& ball, GameObject& two); // Circle -AABB collision 
//float clamp(float value, float min, float max);

Direction VectorDirection(glm::vec2 target); // <collision?, what direction?, difference vector center - closest point>
bool isOtherPowerUpActive(std::vector<PowerUp>& powerUpds, std::string type);

bool ShouldSpawn(unsigned int chance);
void ActivatePowerUp(PowerUp& powerUp);
class Game{
public:
	unsigned int Lives;

	//game state
	GameState State;
	bool Keys[1024];
	bool KeysProcessed[1024];
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
	
	// reset
	void ResetLevel();
	void ResetPlayer();

	std::vector<PowerUp> PowerUps;
	std::vector<uint32_t> sounds;
	void SpawnPowerUps(GameObject& block);
	void UpdatePowerUps(float dt_);

};

#endif // !GAME_H
