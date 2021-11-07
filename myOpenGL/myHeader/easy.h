#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include "cube.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "plane.h"


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
*	6. ���ٻ��Ƹ���ͼ�Σ�������
*		a. cube�������壩
*		b. plane��ƽ�棩
*		c.
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
	void drawPureCube(
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


		static std::string pureCubevShaderCode =
			"#version 430 core\n"
			"layout(location = 0) in vec3 aPos;\n"
			"uniform mat4 model;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
			"}\n"
			;
		static std::string pureCubefShaderCode =
			"#version 430 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"	FragColor = vec4" + glmVec4_to_string(color) + "; \n"
			"}\n"
			;

		// ���ܶ���Ϊȫ�ֱ����������ڶ����ʱ��ͻ����һЩglfw�ĺ��������Ǹ�ʱ��δ����glfwInit()
		static Cube pureCube(CubeType::PURE_CUBE);
		static Shader pureCubeShader(true, pureCubevShaderCode, pureCubefShaderCode);

		pureCubeShader.use();
		pureCubeShader.setMat4("model", model);
		pureCubeShader.setMat4("projection", projection);
		pureCubeShader.setMat4("view", view);
		pureCube.draw(pureCubeShader);
	}

	void drawTextureCube(
		std::string texturePath,
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
	)
	{
		glm::mat4 model, view, projection;
		projection = glm::perspective(glm::radians((double)camera.zoom), 1.0 * windowWidth / windowHeight, 0.1, 100.0);
		view = camera.getViewMatrix();
		model = glm::mat4(1.0f);

		model = glm::translate(model, position);
		model = glm::scale(model, scale);


		static std::string textureCubevShaderCode =
			"#version 430 core\n"
			"layout(location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec2 aTexCoords;\n"
			"out vec2 TexCoords;\n"
			"uniform mat4 model;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
			"	TexCoords = aTexCoords;\n"
			"}\n"
			;
		static std::string textureCubefShaderCode =
			"#version 430 core\n"
			"in vec2 TexCoords;\n"
			"out vec4 FragColor;\n"
			"uniform sampler2D texture1;\n"
			"void main()\n"
			"{\n"
			"	FragColor = texture(texture1, TexCoords);\n"
			"}\n"
			;

		// ���ܶ���Ϊȫ�ֱ����������ڶ����ʱ��ͻ����һЩglfw�ĺ��������Ǹ�ʱ��δ����glfwInit()
		static Cube textureCube(CubeType::TEXTURE_CUBE);
		static Shader textureCubeShader(true, textureCubevShaderCode, textureCubefShaderCode);
		// getTextureֻ����ļ��ж�ȡһ�Σ�֮�󶼻�ֱ�ӷ��ر������texture
		Texture textureCubeTexture0 = Texture::getTexture(texturePath);

		textureCubeShader.use();
		textureCubeShader.setMat4("model", model);
		textureCubeShader.setMat4("projection", projection);
		textureCubeShader.setMat4("view", view);
		textureCubeTexture0.use(0);
		textureCubeShader.setInt("texture1", 0);

		textureCube.draw(textureCubeShader);
	}

	void drawTextureCubeBorder(
		std::string texturePath,
		glm::vec4 borderColor = glm::vec4(0.0, 1.0, 0.0, 1.0),
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
	)
	{
		//---------------------- ���Ʊ߿�Ĺ��� ---------------------------
			// 1.���ȿ�����Ȳ��ԣ�����д��ģ�建��
		glEnable(GL_DEPTH_TEST);
		glStencilMask(0xFF);
		// 2.�趨����ͨ��ģ����ԣ�ͨ����ģ�建��д��1
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		// 3.���ƣ��������Ὣ���������ģ�建������Ϊ1��
		drawTextureCube(texturePath, position, scale);
		// 4.��ֹд��ģ�建�壬�ر���Ȳ���
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		// 5.�趨������1���������ͨ��ģ�����
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		// 6.����һ����һ�����һ����ɫ��������ͼ�Σ���ʱ�ڲ�ģ��Ϊ1�Ľ����ᱻ���ƣ���˵õ��˱߿�Ч��
		drawPureCube(position, scale * glm::vec3(1.1, 1.1, 1.1), borderColor);
		// 7.�ָ�Ĭ�ϣ���ϰ�ߣ�
		glEnable(GL_DEPTH_TEST);
		glStencilMask(0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		// ------------------------------------------------------------------
	}

	// ����plane
	void drawTexturePlane(
		std::string texturePath,
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
		float rotateDegree = 0.0,
		glm::vec3 rotateAxis = glm::vec3(0.0f, 0.0f, 1.0f)
	)
	{
		glm::mat4 model, view, projection;
		projection = glm::perspective(glm::radians((double)camera.zoom), 1.0 * windowWidth / windowHeight, 0.1, 100.0);
		view = camera.getViewMatrix();
		model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::scale(model, scale);
		model = glm::rotate(model, glm::radians(rotateDegree), rotateAxis);

		static std::string texturePlanevShaderCode =
			"#version 430 core\n"
			"layout(location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec2 aTexCoords;\n"
			"out vec2 TexCoords;\n"
			"uniform mat4 model;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
			"	TexCoords = aTexCoords;\n"
			"}\n"
			;
		static std::string texturePlanefShaderCode =
			"#version 430 core\n"
			"in vec2 TexCoords;\n"
			"out vec4 FragColor;\n"
			"uniform sampler2D texture1;\n"
			"void main()\n"
			"{\n"
			//"	FragColor = texture(texture1, TexCoords);\n"
			"vec4 texColor = texture(texture1, TexCoords);\n"
			"if (texColor.a < 0.1)\n"
			"	discard;\n"
			"FragColor = texColor;\n"
			"}\n"
			;

		// ���ܶ���Ϊȫ�ֱ����������ڶ����ʱ��ͻ����һЩglfw�ĺ��������Ǹ�ʱ��δ����glfwInit()
		static Plane texturePlane(PlaneType::TEXTURE_PLANE);
		static Shader texturePlaneShader(true, texturePlanevShaderCode, texturePlanefShaderCode);
		// getTextureֻ����ļ��ж�ȡһ�Σ�֮�󶼻�ֱ�ӷ��ر������texture
		Texture texturePlaneTexture0 = Texture::getTexture(texturePath);

		texturePlaneShader.use();
		texturePlaneShader.setMat4("model", model);
		texturePlaneShader.setMat4("projection", projection);
		texturePlaneShader.setMat4("view", view);
		texturePlaneTexture0.use(0);
		texturePlaneShader.setInt("texture1", 0);

		texturePlane.draw(texturePlaneShader);
	}
}

