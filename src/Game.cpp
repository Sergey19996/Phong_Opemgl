#include "Game.h"
#include "graphics/ResourceManager.h"
#include "graphics/SpriteRenderer.h"
#include "Particle.h"
#include "postProcessor.h"
#include "glm/glm.hpp"
#include "audio/SoundDevice.h"
#include "audio/SoundBuffer.h"
#include "audio/SoundSource.h"
#include <iostream>
#include "TextRenderer.h"
#include "sstream"
// Game-related State data
SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
PostProcessor* Effects;
SoundDevice* MySoundDevice;
TextRenderer* Text;

SoundSource backgroundMusic;
SoundSource objectSpeaker;
SoundSource PowerSpeaker;
SoundSource SolidSpeaker;

const int MAXSPEAKERS = 5;
SoundSource Speakers[MAXSPEAKERS];

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);    // определение коснтант таким образом, будут доступны только тут 
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);      

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;
float ShakeTime = 0.0f;

enum Sounds
{
	S_BACKGROUND, 
	S_BRICKCOLLISION,
	S_POWERUP,
	S_SOLIDCOLLISION,
	S_PADDLESOUND
};


Game::Game(unsigned int width, unsigned int height) : Width(width),Height(height),Keys(),State(GAME_ACTIVE)
{
}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Text;
	delete Effects;

}

void Game::Init(){

	
	MySoundDevice = SoundDevice::get();
	uint32_t /*ALuint*/ Sound1 = SoundBuffer::get()->addSoundEffect("Assets/audio/breakout.mp3");
	uint32_t /*ALuint*/ objectsound = SoundBuffer::get()->addSoundEffect("Assets/audio/bleep.wav");
	uint32_t /*ALuint*/ U_PowerUp = SoundBuffer::get()->addSoundEffect("Assets/audio/powerup.wav");
	uint32_t /*ALuint*/ U_SolidCollision = SoundBuffer::get()->addSoundEffect("Assets/audio/solid.wav");
	uint32_t /*ALuint*/ U_paddleSound = SoundBuffer::get()->addSoundEffect("Assets/audio/bleep.mp3");

	sounds.push_back(Sound1);  //0
	sounds.push_back(objectsound); //1
	sounds.push_back(U_PowerUp); //2
	sounds.push_back(U_SolidCollision);//3
	sounds.push_back(U_paddleSound); // 4
	for (int i = 0; i < MAXSPEAKERS; i++)
	{
		Speakers[i].generate();
	}
	//backgroundMusic.generate();
	//objectSpeaker.generate();
	//PowerSpeaker.generate();
	Speakers[S_BACKGROUND].Play(sounds[S_BACKGROUND]);
	

	//load shaders
	ResourceManager::LoadShader("Assets/Shaders/vertexShader.glsl", "Assets/Shaders/fragmentShader.glsl", nullptr, "sprite");
	ResourceManager::LoadShader("Assets/Shaders/vertexParticleShader.glsl", "Assets/Shaders/fragmentParticleShader.glsl", nullptr, "particle");
	ResourceManager::LoadShader("Assets/Shaders/vertexPost_Processing.glsl", "Assets/Shaders/fragmentPost_Processing.glsl", nullptr, "Post_Processing");
	//configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

	//load textures                                               alpha

	ResourceManager::LoadTexture("Assets/Textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("Assets/Textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("Assets/Textures/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("Assets/Textures/block.png", false, "block");
	ResourceManager::LoadTexture("Assets/Textures/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("Assets/Textures/particle.png", true, "particle");
	ResourceManager::LoadTexture("Assets/Textures/powerup_speed.png", true, "powerup_speed");
	ResourceManager::LoadTexture("Assets/Textures/powerup_sticky.png", true, "powerup_sticky");
	ResourceManager::LoadTexture("Assets/Textures/powerup_increase.png", true, "powerup_increase");
	ResourceManager::LoadTexture("Assets/Textures/powerup_confuse.png", true, "powerup_confuse");
	ResourceManager::LoadTexture("Assets/Textures/powerup_chaos.png", true, "powerup_chaos");
	ResourceManager::LoadTexture("Assets/Textures/powerup_passthrough.png", true, "powerup_passthrough");


	//set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
	Effects = new PostProcessor(ResourceManager::GetShader("Post_Processing"), this->Width, this->Height);

	//load levels
	GameLevel one; one.Load("Assets/levels/one.lvl", this->Width, this->Height / 2);
	GameLevel two; two.Load("Assets/levels/two.lvl", this->Width, this->Height / 2);
	GameLevel three; three.Load("Assets/levels/three.lvl", this->Width, this->Height / 2);
	GameLevel four; four.Load("Assets/levels/four.lvl", this->Width, this->Height / 2);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 0;
	this->Lives = 1;

	// configure game objects
	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));


	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("Assets/fonts/OCRAEXT.ttf", 24);
	
}

void Game::ProcessInput(float dt){

	if (this->State == GAME_WIN) {

		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
		{
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
			Effects->Chaos = false;
			this->State = GAME_MENU;

		}

	}

	if (this->State == GAME_MENU) {

		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {

			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
		}
		if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
		{
			this->Level = (this->Level + 1) % 4;
			this->KeysProcessed[GLFW_KEY_W] = true;
		}
		if (this->Keys[GLFW_KEY_S] && this->KeysProcessed[GLFW_KEY_S])
		{
			if (this->Level > 0)
				--this->Level;
			else
				this->Level = 3;
			this->KeysProcessed[GLFW_KEY_S] = true;


		}

	}

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
	// update particles
	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
	// update PowerUps
	this->UpdatePowerUps(dt);
	// check loss condition
	if (Ball->Position.y >= this->Height) // di ball reach bottom edge?
	{
		--this->Lives;
		//fif the player loses all his lives ? : Game over
		if (this->Lives == 0)
		{
			this->ResetLevel();
			this->State = GAME_MENU;
		}
		this->ResetPlayer();
	}
	// reduce shake time
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
	if (ShakeTime < 0.0f)
		Effects->Shake = false;
	}

	if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted()) {
		this->ResetLevel();
		this->ResetPlayer();
		Effects->Chaos = true;
		this->State = GAME_WIN;


	}

}

void Game::Render()
{
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU)
	{
		Effects->BeginRender();

		//draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
		
		//draw level	
		this->Levels[this->Level].Draw(*Renderer);

		//draw player
		Player->Draw(*Renderer);
		
		//draw ball
		Ball->Draw(*Renderer);
		// draw PowerUps
		for (PowerUp& powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);
		//draw particles
		Particles->Draw();
		// end rendering to postprocessing framebuffer
		Effects->EndRender();
		// render postprocessing quad
		Effects->Render(glfwGetTime());

		std::stringstream ss; ss << this -> Lives;
		Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	if (this->State == GAME_MENU) {

		Text->RenderText("Press ENTER to start", 250.0f, Height / 2, 1.0f);
		Text->RenderText("Press W or S to select level", 245.0f, Height / 2 + 20.0f, 0.75f);


	}
	if (this->State == GAME_WIN) {

		Text->RenderText("You Won!!!", 320.0, Height / 2 - 20.0, 1.0, glm::vec3(0.0f, 1.0f, 0.0f));
		Text->RenderText("Press ENTER to retry or Escape for quit", 130.0, Height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0));


	}
	
}

void Game::DoCollisions(){

	for (GameObject& box : this->Levels[this->Level].Bricks) {   // бежим по жЄстким и м€гким кирпичам

		if (!box.Destroyed) {

			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision)) // if collision is true
			{
			
			
				//destroy block if not solid
				if (!box.IsSolid ) {
					box.Destroyed = true;
					this->SpawnPowerUps(box);
					Speakers[S_BRICKCOLLISION].Play(sounds[S_BRICKCOLLISION]);
				}
				else
				{//if block is solid, enable shake effect
					ShakeTime = 0.05f;
					Effects->Shake = true;
					Speakers[S_SOLIDCOLLISION].Play(sounds[S_SOLIDCOLLISION]);
				}
				//
					//collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (!(Ball->PassThrough && !box.IsSolid)) // don't do collision resolution on non-solid bricks if pass-through is activated
				if (dir == LEFT || dir == RIGHT) // horizontal collision
				{
					
					Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
					//relocate 
					float penetration = Ball->Radius - std::abs(diff_vector.x); // ќтнимаем от радиуса рассто€ние до ближайшей точки на AABB
					if (dir == LEFT)
						Ball->Position.x += penetration; //move ball to right
					else
						Ball->Position.x -= penetration;// move ball to left
				}
				else // vertical collision
				{
					Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
					//relocate 
					float penetration = Ball->Radius - std::abs(diff_vector.y);
					if (dir == UP)
						Ball->Position.y -= penetration;  // ќтн€ть = подн€ть выше
					else
						Ball->Position.y += penetration; //  прибавить = опустить ниже


				}
				//
				

			}
			// check collisions for player pad (unless stuck)
			Collision result = CheckCollision(*Ball, *Player);
			if (!Ball->Stuck && std::get<0>(result))
			{

				//check where it hit the board, and change velocity based on where it hit the board
				float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
				float distance = (Ball->Position.x + Ball->Radius) - centerBoard; // от центра борда до центра круга
				float percentage = distance / (Player->Size.x / 2.0f);
				//then move accordingly
				float strength = 2.0f;
				glm::vec2 oldVelocity = Ball->Velocity;
				Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
				Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
				Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // удлинит 
				Ball->Stuck = Ball->Sticky;
				Speakers[S_PADDLESOUND].Play(sounds[S_PADDLESOUND]);
			}



		}

	}
	// also check collisions on PowerUps and if so, activate them
	for (PowerUp& powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed) {
			
			// first check if powerup passed bottom edge, if so: keep as inactive and destroy
			if (powerUp.Position.y >= this->Height) // если поверап ниже ширирны экрана
				powerUp.Destroyed = true;

			if (CheckCollision(*Player, powerUp)) {
				// collided with player, now activate powerup
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = true;
				powerUp.Activated = true;
				Speakers[S_POWERUP].Play(sounds[S_POWERUP]);

			}


		}
	}


}

void Game::ResetLevel(){
	if (this->Level == 0) 
		this->Levels[0].Load("Assets/levels/one.lvl", this->Width, this->Height / 2);
	else if (this->Level == 1)
		this->Levels[1].Load("Assets/levels/.lvl", this->Width, this->Height / 2);
	else if (this->Level == 2)
		this->Levels[2].Load("Assets/levels/.lvl", this->Width, this->Height / 2);
	else if (this->Level == 3)
		this->Levels[3].Load("Assets/levels/.lvl", this->Width, this->Height / 2);
	this->Lives = 3;
}

void Game::ResetPlayer(){
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y); // дл€ ракетки центр низ центра уровн€ 
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY); // дл€ меча центр ракетки

}

void Game::SpawnPowerUps(GameObject& block){
	if (ShouldSpawn(75)) { // 1/75
		this->PowerUps.push_back(
			PowerUp("speed", glm::vec3(0.5f,0.5f,1.0f),0.0f,block.Position,ResourceManager::GetTexture("powerup_speed")
		));

	}
	if (ShouldSpawn(75)) { // 1/75
		this->PowerUps.push_back(
			PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")
		));

	}
	if (ShouldSpawn(75)) { // 1/75
		this->PowerUps.push_back(
			PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")
		));

	}
	if (ShouldSpawn(75)) { // 1/75
		this->PowerUps.push_back(
			PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")
		));

	}
	if (ShouldSpawn(15)) { // negative powerups should spawn more often
		this->PowerUps.push_back(
			PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")
		));

	}
	if (ShouldSpawn(15)) { // negative powerups should spawn more often
		this->PowerUps.push_back(
			PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")
		));

	}
}

void Game::UpdatePowerUps(float dt_)
{
	for (PowerUp& powerUp : this->PowerUps) {

		powerUp.Position += powerUp.Velocity * dt_;
		if (powerUp.Activated) {
			powerUp.Duration -= dt_;
			if (powerUp.Duration <= 0.0f) {
				//remove powerup from list
				powerUp.Activated = false;
				//deactivate effects
				if (powerUp.Type == "sticky"){
					if (!isOtherPowerUpActive(this->PowerUps, "sticky")) {
						//only reset it if no other PowerUp of type is active
						Ball->Sticky = false;
						Player->Color = glm::vec3(1.0f);

					}

					}
				else if (powerUp.Type == "pass-through")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "pass-through"))
					{	// only reset if no other PowerUp of type pass-through is active
						Ball->PassThrough = false;
						Ball->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "confuse"))
					{	// only reset if no other PowerUp of type confuse is active
						Effects->Confuse = false;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!isOtherPowerUpActive(this->PowerUps, "chaos"))
					{	// only reset if no other PowerUp of type chaos is active
						Effects->Chaos = false;
					}
				}

			}

		}

	}
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(), [](const PowerUp& powerUp) {
		return powerUp.Destroyed && !powerUp.Activated;
		}), this->PowerUps.end());


}

bool CheckCollision(GameObject& one, GameObject& two)
{
	//AABB - AABB
	bool x = one.Position.x + one.Size.x >= two.Position.x && one.Position.x <= two.Position.x + two.Size.x;
	bool y = one.Position.y + one.Size.y >= two.Position.y && one.Position.y <= two.Position.y + two.Size.y;




	return x && y;

}

Collision CheckCollision(BallObject& ball, GameObject& two)
{
	//get center point circle first
	glm::vec2 center(ball.Position + ball.Radius);
	//calculate AABB info (center, half-externs)
	glm::vec2 aabb_half_externs(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabb_center(two.Position + aabb_half_externs); // центр квадрата 

	//get difference vectr between both centers
	glm::vec2 difference = center - aabb_center; // вектор от центра квадрата до центра круга 

	glm::vec2 clamped = glm::clamp(difference, -aabb_half_externs, aabb_half_externs);
	//add clamped vector to aabb_center and we get the value of box closest to circle
	glm::vec2 closest = aabb_center + clamped;
	//retrive(извельч) vector between center circle and closest point AABB and check who longer
	difference = closest - center;  // вектор от центра круга до ближайшей точки 

	if (glm::length(difference) <= ball.Radius)
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));

	


}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f,1.0f),  // Up
		glm::vec2(1.0f,0.0f),  // right
		glm::vec2(0.0f,-1.0f), // Down
		glm::vec2(-1.0f,0.0f)  // Left
	};
	float max = 0.0f; // cos 0 - перпендикул€р в dot
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++) {

		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}


	return (Direction)best_match; // на какую сторону проекци€ дала ближе к 1, то и вернЄт
}

bool isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)  // когд€ € беру одит и тот же апгрейд
{
	for (const PowerUp& powerUp : powerUps) {

		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;

	}

	return false;
}

bool ShouldSpawn(unsigned int chance)
{
	unsigned int random = rand() % chance;
	return random == 0;
}

void ActivatePowerUp(PowerUp& powerUp){
	if (powerUp.Type == "speed")
	{
		Ball->Velocity *= 1.2;

	}
	else if (powerUp.Type == "sticky"){
		Ball->Sticky = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);

	}
	else if (powerUp.Type == "pass-through") {
		Ball->PassThrough= true;
		Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);

	}
	else if (powerUp.Type == "pad-size-increase") {
		Player->Size.x += 50;

	}
	else if (powerUp.Type == "confuse") {
		if (!Effects->Chaos)
			Effects->Confuse = true; // only activate if chaos wasn't already active

	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Confuse)
			Effects->Chaos = true;
	}
}

//float clamp(float value, float min, float max)
//{
//	return std::max(min, std::min(max, value));
//}
