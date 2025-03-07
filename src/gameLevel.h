#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>


#include "gameobject.h"

#include "graphics/ResourceManager.h" //singelton
#include "graphics/SpriteRenderer.h"
class GameLevel
{
public:
	//level state 
	std::vector<GameObject> Bricks;
	//constructor
	GameLevel() { }
	//load level from file
	void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);
	//render level
	void Draw(SpriteRenderer& render);
	//check if the level si completed (all non-solid block are desptryed)
	bool IsCompleted();
private:
	//initialzie level from tile data
	void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);



};

#endif // !GAME_LEVEL_H
