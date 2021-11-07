#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "myHeader/cube.h"
#include "myHeader/model.h"
#include "myHeader/shader.h"
#include "myHeader/texture.h"
#include "myHeader/easy.h"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		glDepthFunc(GL_LESS);
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		glDepthFunc(GL_ALWAYS);
	}
}

int main()
{
	EASY::init("hard", glm::vec3(0, 0, 4));

	glfwSetKeyCallback(EASY::window, key_callback);

	glEnable(GL_STENCIL_TEST);

	while (!glfwWindowShouldClose(EASY::window))
	{
		EASY::oneFrame();
		glClear(GL_STENCIL_BUFFER_BIT);

		EASY::drawTexturePlane("resources/wall.jpg", glm::vec3(0.0, -0.5, 0.0), glm::vec3(5, 5, 5));

		EASY::drawTextureCubeBorder("resources/container.jpg", glm::vec4(0, 1, 0, 1), glm::vec3(-2.0, 0, 0), glm::vec3(1.0));
		EASY::drawTextureCubeBorder("resources/container.jpg", glm::vec4(0, 1, 0, 1), glm::vec3(0.0, 0, 0), glm::vec3(1.0));

		EASY::drawTexturePlane("resources/grass.png", glm::vec3(1, 0.5, 1), glm::vec3(1), -90, glm::vec3(1, 0, 0));
	}

	EASY::terminate();

	return 0;
}

