#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include "cube.h"
#include "camera.h"
#include "shader.h"


/*
* 这是一个非常方便的头文件。
*
* include这个头文件并且调用EASY::init()之后，就会设置好一个window、一个摄像机、一些方便的绘制函数
* 在每一帧应当调用一次oneFrame()，用于更新
*
* EasyWindow包含以下控件和功能：
*	1. 一个window
*	2. 一个camera
*	3. window大小可改变
*	4. 通过adws控制camera移动
*	5. 滚轮进行视野缩放，鼠标移动进行camera旋转
*	6. 快速绘制各种图形
*
*/


namespace EASY
{
	GLFWwindow* window;				// 窗口
	int windowWidth = 1200;
	int windowHeight = 800;

	Camera camera;					// 相机

	float deltaTime = 0.0f;		// 时间间隔
	float lastFrame = 0.0f;

	// mouse_callback 需要的一些参数
	float lastX;
	float lastY;
	bool firstMouse = true;


	// 一些callback和输入处理
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
		// 初始化glfw
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// 创建glfw窗口
		window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun)framebuffer_size_callback);
		// glad 加载所有opengl的函数指针
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		// 开启深度测试（Z-buffer)
		glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_ALWAYS);
		//glDepthMask(GL_FALSE);
	}



	//-----------------------------------------------------------------------------------------
	// 前面的函数一般不会从外部调用


	// 初始化函数，要使用EASY必须先调用
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

	// 逐帧处理函数，外部必须每一帧调用
	void oneFrame()
	{
		// 交换缓冲
		glfwPollEvents();
		glfwSwapBuffers(window);

		// 更新时间间隔
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 处理输入
		processInput(window);

		// 清理window
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// 将glm::vec4转换为string
	// 例如glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)会变成"(1.0, 1.0, 1.0, 1.0)"
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

	// 绘制 cube
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

		// 不能定义为全局变量，否则在定义的时候就会调用一些glfw的函数，而那个时候还未进行glfwInit()
		static Cube cube;
		Shader cubeShader(true, vShaderCode, fShaderCode);

		cubeShader.use();
		cubeShader.setMat4("model", model);
		cubeShader.setMat4("projection", projection);
		cubeShader.setMat4("view", view);
		cube.draw(cubeShader);
	}
}

