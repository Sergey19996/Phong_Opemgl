#include "Game.h"
#include "graphics/ResourceManager.h"
#include "graphics/SpriteRenderer.h"

// Game-related State data
SpriteRenderer* Renderer;
GameObject* Player;



Game::Game(unsigned int width, unsigned int height) : Width(width),Height(height),Keys(),State(GAME_ACTIVE)
{
}

Game::~Game()
{
	delete Renderer;
	delete Player;
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


	//load textures                                               alpha

	ResourceManager::LoadTexture("Assets/Textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("Assets/Textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("Assets/Textures/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("Assets/Textures/block.png", false, "block");
	ResourceManager::LoadTexture("Assets/Textures/block_solid.png", false, "block_solid");

	//load levels
	GameLevel one; one.Load("Assets/levels/one.lvl", this->Width, this->Height / 2);
	GameLevel two; two.Load("Assets/levels/two.lvl", this->Width, this->Height / 2);
	GameLevel three; three.Load("Assets/levels/three.lvl", this->Width, this->Height / 2);
	GameLevel four; four.Load("Assets/levels/four.lvl", this->Width, this->Height / 2);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);


	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,this->Height - PLAYER_SIZE.y);

	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
}

void Game::ProcessInput(float dt){

	if (this->State == GAME_ACTIVE) {

		float velocity = PLAYER_VELOCITY * dt;
		//move playerboard
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f)
				Player->Position.x -= velocity;

		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x >= 0.0f)
				Player->Position.x += velocity;

		}


	}



}

void Game::Update(float dt)
{
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		//draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
	}
	//draw level	
	this->Levels[this->Level].Draw(*Renderer);
	Player->Draw(*Renderer);
}
