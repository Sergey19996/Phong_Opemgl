#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

#include <glad/glad.h> // opengl core
#include <glm/glm.hpp> // mat

#include "graphics/rendering/Texture.h"
#include "graphics/SpriteRenderer.h"
#include "graphics/rendering/Shader.h"


// PostProcessor hosts all PostProcessing effects for BreackOut
//Game.It renders the game on a textured quad after each one can
// enable specific effects by enabling either the Confuse, Chaos or
//Shake boolean.
//It is required to call BeginRender() before rendering the game
//and EndRender() after rendering the game for the class to work

class PostProcessor {
public:
	//state
	Shader PostProcessingShader;
	Texture2D Texture;
	unsigned int Width, Height;
	//options
	bool Confuse, Chaos, Shake;
	//constructor
	PostProcessor(Shader shader, unsigned int width, unsigned int height);
	//prepare the postprocessors framebuffer operations before rendering the game
	void BeginRender();
	//should be called after rendering the game, so it stores all the rendering data into to texture object
	void EndRender();
	//renders the PostProcessor texture quad (as a screen-encompassing large sprite)
	void Render(float time);

private:
	//render state
	unsigned int MSFBO, FBO; // MSFBO = multisampled FBO.FBO is regular, used for blitting MS color-buffer to texture
	unsigned int RBO; //RBO is use for multisampled color buffer
	unsigned int VAO; 
	// initialize quad for rendering postprocessing texture
	void initRenderData();





};



#endif // !POSTPROCESSOR_H
