#ifndef BALL_OBECT_H
#define BALL_OBJECT_H

#include <glad/glad.h> //рендер
#include <glm/glm.hpp> // математика


#include "gameobject.h"
#include "graphics/rendering/Texture.h"


class BallObject : public GameObject {

public:
	//ball state
	float Radius;
	bool Stuck;

	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

	glm::vec2 Move(float dt, unsigned int Widnow_width);
	void Reset(glm::vec2 position, glm::vec2 velocity);

};

#endif // !BALL_OBECT_h
