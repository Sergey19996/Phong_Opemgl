#include "Game.h"
#include "graphics/ResourceManager.h"
#include "graphics/SpriteRenderer.h"

// Game-related State data
SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);    // определение коснтант таким образом, будут доступны только тут 
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);      

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;


Game::Game(unsigned int width, unsigned int height) : Width(width),Height(height),Keys(),State(GAME_ACTIVE)
{
}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
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

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

}

void Game::ProcessInput(float dt){

	if (this->State == GAME_ACTIVE) {

		float velocity = PLAYER_VELOCITY * dt;
		//move playerboard
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f) {
				Player->Position.x -= velocity;
				if (Ball->Stuck)
					Ball->Position.x -= velocity;

			}



		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;

			}


		}
		if (this->Keys[GLFW_KEY_SPACE]) {
			Ball->Stuck = false;


		}

	}



}

void Game::Update(float dt)
{
	//update objects
	Ball->Move(dt, this->Width);
	//check for collision
	this->DoCollisions();

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
	Ball->Draw(*Renderer);
	Player->Draw(*Renderer);
}

void Game::DoCollisions(){

	for (GameObject& box : this->Levels[this->Level].Bricks) {

		if (!box.Destroyed) {

			if (CheckCollision(*Ball, box))
			{
				if (!box.IsSolid) {
					box.Destroyed = true;
				}
			}

		}

	}

}

bool CheckCollision(GameObject& one, GameObject& two)
{
	//AABB - AABB
	bool x = one.Position.x + one.Size.x >= two.Position.x && one.Position.x <= two.Position.x + two.Size.x;
	bool y = one.Position.y + one.Size.y >= two.Position.y && one.Position.y <= two.Position.y + two.Size.y;




	return x && y;

}

bool CheckCollision(BallObject& ball, GameObject& two)
{
	//get center point circle first
	glm::vec2 center(ball.Position + ball.Radius);
	//calculate AABB info (center, half-externs)
	glm::vec2 aabb_half_externs(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabb_center(two.Position + aabb_half_externs);

	//get difference vectr between both centers
	glm::vec2 difference = center - aabb_center; // вектор от центра квадрата до центра круга 

	glm::vec2 clamped = glm::clamp(difference, -aabb_half_externs, aabb_half_externs);
	//add clamped vector to aabb_center and we get the value of box closest to circle
	glm::vec2 closest = aabb_center + clamped;
	//retrive(извельч) vector between center circle and closest point AABB and check who longer
	difference = closest - center; 
	return glm::length(difference) < ball.Radius ;



}

//float clamp(float value, float min, float max)
//{
//	return std::max(min, std::min(max, value));
//}
