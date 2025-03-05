#include "Shader.h"

#include <iostream>

Shader& Shader::Use()
{
	glUseProgram(this->ID);
	return* this;
}

void Shader::Compile(const char* VertexSource, const char* FragmentSource, const char* GeometrySource){

	unsigned int sVertex, sFragment, gShader;
	//vertex Shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);  // create empty shader object behind the scene
	glShaderSource(sVertex, 1, &VertexSource, NULL); //Загружаете исходный код шейдера в этот объект
	glCompileShader(sVertex); // compile
	checkCompileErrors(sVertex, "VERTEX");
	//fragment Shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &FragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");
	//if geometry shader source code is given, also compile geometry shader
	if (GeometrySource != nullptr) {
		gShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShader, 1, &GeometrySource, NULL);
		glCompileShader(gShader);
		checkCompileErrors(gShader, "GEOMETRY");
	}
	//shader program
	this->ID = glCreateProgram();
	glAttachShader(this->ID, sVertex);
	glAttachShader(this->ID, sFragment);
	if (GeometrySource != nullptr)
	glAttachShader(this->ID,gShader);

	glLinkProgram(this->ID);
	checkCompileErrors(this->ID, "PROGRAM");
	//delete the shaders as they're linked it out program and no longer necessery
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (GeometrySource != nullptr)
		glDeleteShader(gShader);


}

void Shader::SetFloat(const char* name, float value, bool useShader){
	if (useShader)
	this->Use();
	glUniform1f(glGetUniformLocation(this->ID, name), value);

}

void Shader::SetInteger(const char* name, int value, bool useShader){
	if (useShader)
		this->Use();
	glUniform1f(glGetUniformLocation(this->ID, name), value);
}

void Shader::SetVector2f(const char* name, float x, float y, bool useShader){
	if (useShader)
		this->Use();
	glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}

void Shader::SetVector2f(const char* name, const glm::vec2& value, bool useShader){
	if (useShader)
		this->Use();
	glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}

void Shader::SetVector3f(const char* name, float x, float y, float z, bool useShader){
	if (useShader)
		this->Use();
	glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}

void Shader::SetVector3f(const char* name, const glm::vec3& value, bool useShader){
	if (useShader)
		this->Use();
	glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}

void Shader::SetVector4f(const char* name, float x, float y, float z, float w, bool useShader){
	if (useShader)
		this->Use();
	glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}

void Shader::SetVector4f(const char* name, const glm::vec4& value, bool useShader){
	if (useShader)
		this->Use();
	glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}

void Shader::SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader){

	if (useShader)
		this->Use();
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(unsigned int object, std::string type){

	int success;
	char infoLog[1024];
	if (type != "PROGRAM") {

		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type:" << type << "\n"
				<< infoLog << "\n --  ------------------------------------------- -- "
				<< std::endl;

		}

	}
	else{

		glGetProgramiv(object, GL_LINK_STATUS, &success); //используется для получения информации о состоянии шейдерной программы
		if (!success)
		{

			glGetProgramInfoLog(object, 1024, NULL, infoLog);


		}


	}

}
