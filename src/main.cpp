#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h> // библиотека для работы с окнами, контекстами OpenGL и входными устройствами (клавиатура, мышь, джойстики и т. д.).
#include "Game.h"
#include "graphics/ResourceManager.h"

//bool checkFFmpegConnection() {
//	// В последних версиях FFmpeg регистрация не требуется
//	// av_register_all();  // Убираем это для новых версий
//
//	// Получение информации о версии FFmpeg
//	const char* version = av_version_info();
//
//	// Проверка, получена ли информация о версии
//	if (version) {
//		std::cout << "Success" << std::endl;
//		return true;
//	}
//	else {
//		std::cerr << "Wrong" << std::endl;
//		return false;
//	}
//}


//GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height); //изменение размера окна
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode); //событие на нажатие с клавиатуры

//the width of the screen 
const unsigned int SCREEN_WIDTH = 800;
//the height of the screen
const unsigned int SCREEN_HEIGHT = 600;
Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);





int main(int argc, char* argv[]) {



	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  //3.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // .3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // !__APPLE__

		glfwWindowHint(GLFW_RESIZABLE, false); // можно ли скейлить окно
		GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
		glfwMakeContextCurrent(window);

		glfwSetKeyCallback(window, key_callback);  //подключаем клавиатуру
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //подключаем возможность скейлить окно
		glfwSwapInterval(1);
		//glad: load all opengl function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;

		}
		//opengl configurations
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//initialize game
		Breakout.Init();

		//deltatime variables
		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		while (!glfwWindowShouldClose(window))
		{
			//calculate delta time
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			glfwPollEvents(); // обновление на скейл и на нажатия программы

			//manage user input
			Breakout.ProcessInput(deltaTime); 

			

			//update game state
			Breakout.Update(deltaTime);

			//render
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			Breakout.Render();

			glfwSwapBuffers(window);

		}

		//delete ALL resources as loaded using  in resource manager
		ResourceManager::clear();

		glfwTerminate();


	return 0;

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (key >= 0 && key < 1024) {

		if (action == GLFW_PRESS)
			Breakout.Keys[key] = true;
		else if (action == GLFW_RELEASE)
			Breakout.Keys[key] = false;


		

	}

}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
   // height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}