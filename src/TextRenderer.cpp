#include "TextRenderer.h"
#include <iostream>

#include "glm/gtc/matrix_transform.hpp"

#include "ft2build.h"
#include FT_FREETYPE_H

#include "TextRenderer.h"
#include "graphics/ResourceManager.h"

TextRenderer::TextRenderer(unsigned int width, unsigned int height){
	//load and configure shader
	TextShader = ResourceManager::LoadShader("Assets/Shaders/text_2d_vs.glsl", "Assets/Shaders/text_2d_fs.glsl", nullptr, "text");
	TextShader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f), true);
	TextShader.SetInteger("text", 0);
	//configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW); // 6 vertices rectangle has and pos =x;y; uv =s;t
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0); // layout(location = 0)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


}

void TextRenderer::Load(std::string font, unsigned int fontSize)
{
	
	//first clear the previously loaded Characters
	Characters.clear();
	//then initialize and load the FreeType library
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) // all functionss return value as 0 whenever an error occured
		std::cout << "ERROR::FREETYPE: Couldn't init freeTYpe library" << std::endl;
	//load font as face
	FT_Face face;
	if (FT_New_Face(ft, font.c_str(), 0, &face)) 
	std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	//set size to load glyph as
	FT_Set_Pixel_Sizes(face, 0, fontSize);
	//disable byte-alignment restriction(�����������)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//then for the first 1280 ASCII characters, pre-load/compile their characters and store them
	for (GLubyte c = 0; c < 128; c++) // load see what i did there
	{
		//load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYPE: failed to load GLyph" << std::endl;
			continue;
		}
		//generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		//set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left,face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));

	}
	glBindTexture(GL_TEXTURE_2D, 0);
	//Destroy freetype once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{//activate corresponding render state
	this->TextShader.Use();
	this->TextShader.SetVector3f("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->VAO);

	//iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {

		Character ch = Characters[*c];
		float xpos = x + ch.Bearing.x * scale;
		float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale; // � H bearing y �������� ������� �����(����� ���� ��������) ��� � � � T

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		//update VBO for each character
		float vertices[6][4] = {
			{xpos, ypos + h,	 0.0f, 1.0f},
			{xpos + w, ypos,	 1.0f, 0.0f},
			{xpos, ypos,		 0.0f, 0.0f},

			{xpos, ypos + h,	 0.0f, 1.0f},
			{xpos + w, ypos + h, 1.0f, 1.0f},
			{xpos + w, ypos ,    1.0f, 0.0f},
		};
		//render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureId);
		//update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//now advance cursor to the next glyph
		x += (ch.Advance >> 6) * scale; //bitshif by 6 to get value in pixels (1/64th times 2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
