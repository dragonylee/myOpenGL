#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include "cube.h"
#include "camera.h"
#include "shader.h"


/*
* ����һ���ǳ������ͷ�ļ���
*
* include���ͷ�ļ����ҵ���EASY::init()֮�󣬾ͻ����ú�һ��window��һ���������һЩ����Ļ��ƺ���
* ��ÿһ֡Ӧ������һ��oneFrame()�����ڸ���
*
* EasyWindow�������¿ؼ��͹��ܣ�
*	1. һ��window
*	2. һ��camera
*	3. window��С�ɸı�
*	4. ͨ��adws����camera�ƶ�
*	5. ���ֽ�����Ұ���ţ�����ƶ�����camera��ת
*	6. ���ٻ��Ƹ���ͼ��
*
*/


namespace EASY
{
	GLFWwindow* window;				// ����
	int windowWidth = 1200;
	int windowHeight = 800;

	Camera camera;					// ���

	float deltaTime = 0.0f;		// ʱ����
	float lastFrame = 0.0f;

	// mouse_callback ��Ҫ��һЩ����
	float lastX;
	float lastY;
	bool firstMouse = true;


	// һЩcallback�����봦��
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.processKeyBoard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.processKeyBoard(BACKWORD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.processKeyBoard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.processKeyBoard(RIGHT, deltaTime);
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
			return;
		}

		float xOffset = xpos - lastX;
		float yOffset = ypos - lastY;
		lastX = xpos;
		lastY = ypos;

		camera.processMouseMovement(xOffset, yOffset);
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera.processMouseScroll(yoffset);
	}

	void setupWindow(std::string title)
	{
		// ��ʼ��glfw
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// ����glfw����
		window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun)framebuffer_size_callback);
		// glad ��������opengl�ĺ���ָ��
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		// ������Ȳ��ԣ�Z-buffer)
		glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_ALWAYS);
		//glDepthMask(GL_FALSE);
	}



	//-----------------------------------------------------------------------------------------
	// ǰ��ĺ���һ�㲻����ⲿ����


	// ��ʼ��������Ҫʹ��EASY�����ȵ���
	void init(
		std::string windowTitle = "hhhhh",
		glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 2.0f)
	)
	{
		camera = Camera(cameraPosition);
		setupWindow(windowTitle);
	}

	void terminate()
	{
		glfwTerminate();
	}

	// ��֡���������ⲿ����ÿһ֡����
	void oneFrame()
	{
		// ��������
		glfwPollEvents();
		glfwSwapBuffers(window);

		// ����ʱ����
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ��������
		processInput(window);

		// ����window
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// ��glm::vec4ת��Ϊstring
	// ����glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)����"(1.0, 1.0, 1.0, 1.0)"
	std::string glmVec4_to_string(glm::vec4 vec)
	{
		std::string s = "(";
		s += std::to_string(vec.x);
		s += ", ";
		s += std::to_string(vec.y);
		s += ", ";
		s += std::to_string(vec.z);
		s += ", ";
		s += std::to_string(vec.w);
		s += ")";
		return s;
	}

	// ���� cube
	void drawCube(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
	)
	{
		glm::mat4 model, view, projection;
		projection = glm::perspective(glm::radians((double)camera.zoom), 1.0 * windowWidth / windowHeight, 0.1, 100.0);
		view = camera.getViewMatrix();
		model = glm::mat4(1.0f);

		model = glm::translate(model, position);
		model = glm::scale(model, scale);

		
		std::string vShaderCode =
			"#version 430 core\n"
			"layout(location = 0) in vec3 aPos;\n"
			"out vec3 FragPos;\n"
			"uniform mat4 model;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
			"	FragPos = vec3(model * vec4(aPos, 1.0));\n"
			"}\n"
			;
		std::string fShaderCode =
			"#version 430 core\n"
			"in vec3 FragPos;\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"	FragColor = vec4" + glmVec4_to_string(color) + "; \n"
			"}\n"
			;

		// ���ܶ���Ϊȫ�ֱ����������ڶ����ʱ��ͻ����һЩglfw�ĺ��������Ǹ�ʱ��δ����glfwInit()
		static Cube cube;
		Shader cubeShader(true, vShaderCode, fShaderCode);

		cubeShader.use();
		cubeShader.setMat4("model", model);
		cubeShader.setMat4("projection", projection);
		cubeShader.setMat4("view", view);
		cube.draw(cubeShader);
	}
}

