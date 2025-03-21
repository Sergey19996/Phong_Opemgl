#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H
#include <map>

//#include "glad/glad.h"
#include "glm/glm.hpp"  //math

#include "graphics/rendering/Texture.h"
#include "graphics/rendering/Shader.h"

///Holds all state information relevant to a character as loaded using FreeType
struct Character{
	unsigned int TextureId; //ID handle of the glyph texture
	glm::vec2 Size; 
	glm::vec2 Bearing; // offset from baseline to left/top of glyph
	unsigned int Advance; // horizontal offset to advance to next glyph
};

//A renderer class for rendering text displayed by a font loaded using the
//FreeType library.A single fonr is loaded, procecced into a list of Character
//items for later rendering;
class TextRenderer {
public:
	//holds a list of pre-compiled Characters
	std::map<char, Character> Characters;
	//shader used for text renderer
	Shader TextShader;
	//constructor
	TextRenderer(unsigned int width, unsigned int height);
	//pre-cimpiles a list of characters from the given font
	void Load(std::string font, unsigned int fontSize);
	//renders a string of text using the precompiled list of characters
	void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));
private:
	//render state
	unsigned int VAO, VBO;




};


#endif // !TEXT_RENDERER_H
