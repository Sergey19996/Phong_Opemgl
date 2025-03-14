#ifndef PARTICLE_H
#define PARTICLE_H
#include <vector>


#include <glm/glm.hpp>
#include <glad/glad.h>

#include "graphics/rendering/Shader.h"
#include "graphics/rendering/Texture.h"
#include "gameobject.h"


//represent single particle and it's state
struct  Particle
{
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	float Life;

	Particle() :
		Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};


//Particle generator acts as a container for rendering a large number of
//particles by repeatedly spawning and updating particles and killing
// them after a given amount of time

class ParticleGenerator
{
public:
	//constructor
	ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
	// update all particles
	void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
	//render all particles
	void Draw();
private:
	//state 
	std::vector<Particle> particles;
	unsigned int amount;
	//render state
	Shader shader;
	Texture2D texture;
	unsigned int VAO;
	//initialize buffer and vertex attribute
	void init();
	//retunr particle index that currently unused e.g Life <= 0.0f or 0 if no particle is currently inactive
	unsigned int firstUnusedParticle();
	//respawns particles
	void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));




};



#endif // !PARTICLE_H
