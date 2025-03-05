#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>
#include <glad/glad.h>

#include "rendering/texture.h"
#include "rendering/Shader.h"

// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no 
// public constructor is defined.

class ResourceManager {
public:
	//reource storaage
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Texture2D> Textures;
	//load (and generates) a shader program from file loading vertex, fregment (and geometry) shader's source code. if gShaderFile is not nullptr, it aslo loads a geometry shader
	static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
	//retrieves a stored shader (извлекает)
	static Shader GetShader(std::string name);
	//load (and generate) a texture from file
	static Texture2D LoadTexture(const char* file, bool alpha, std::string name);
	//retrieves a stored texture
	static Texture2D GetTexture(std::string name);
	//properly de-allocates all loaded resources
	static void clear();

private:
	//private constructor, that is we do'nt want any actual resource manager objects, it's members and functions should be publicy avaialbe (static).
	ResourceManager() {}
	//loads and generate a shader from file
	static Shader loadShaderFromFile(const char* vShadderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
	// loads a single texture from file
	static Texture2D loadTextureFromFIle(const char* File, bool alpha);


};

#endif // !RESOURCE_MANAGER_H
