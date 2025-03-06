#include "Game.h"
#include "graphics/ResourceManager.h"
#include "graphics/SpriteRenderer.h"


// Game-related State data
SpriteRenderer* Renderer;

Game::Game(unsigned int width, unsigned int height) : Width(width),Height(height),Keys(),State(GAME_ACTIVE)
{
}

Game::~Game()
{
	delete Renderer;
}

void Game::Init(){

	//load shaders
	ResourceManager::LoadShader("Assets/Shaders/vertexShader.glsl", "Assets/Shaders/fragmentShader.glsl", nullptr, "sprite");
	//configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	//set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	//load textures
	ResourceManager::LoadTexture("Assets/Textures/awesomeface.png", true, "face");
}

void Game::ProcessInput(float dt)
{
}

void Game::Update(float dt)
{
}

void Game::Render()
{
	Renderer->DrawSprite(ResourceManager::GetTexture("face"),
		glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));

}
