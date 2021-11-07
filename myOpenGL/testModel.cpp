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


int isSpotLight = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		// 控制聚光灯
		isSpotLight = 1 - isSpotLight;
	}
}

int main()
{
	EASY::init();	// 必须

	glfwSetKeyCallback(EASY::window, key_callback);

	Model ourModel("C:/Users/Yinglong Li/Downloads/pearl.fbx", true);
	Shader shader("shaders/model.vshader", "shaders/model.fshader");

	glm::vec3 lightPos(5.0f, 13.0f, 0.0f);

	while (!glfwWindowShouldClose(EASY::window))	// 必须
	{
		EASY::oneFrame();	// 必须


		// 绘制模型
		glm::mat4 model, projection, view;

		projection = glm::perspective(glm::radians((double)EASY::camera.zoom), 1.0 * EASY::windowWidth / EASY::windowHeight, 0.1, 100.0);
		view = EASY::camera.getViewMatrix();
		model = glm::mat4(1.0f);

		shader.use();

		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		shader.setMat4("model", model);

		// 光照
		shader.setFloat("material.shininess", 32.0f);
		shader.setVec3("viewPos", EASY::camera.position);

		// 控制聚光灯
		shader.setInt("isSpotLight", isSpotLight);

		// 定向光
		shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// 聚光灯
		shader.setVec3("spotLight.position", EASY::camera.position);
		shader.setVec3("spotLight.direction", EASY::camera.front);
		shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("spotLight.constant", 1.0f);
		shader.setFloat("spotLight.linear", 0.009);
		shader.setFloat("spotLight.quadratic", 0.0032);
		shader.setFloat("spotLight.innerCos", glm::cos(glm::radians(12.5f)));
		shader.setFloat("spotLight.outerCos", glm::cos(glm::radians(15.0f)));
		// 点光源 1
		shader.setVec3("pointLights[0].position", lightPos);
		shader.setVec3("pointLights[0].ambient", 0.0f, 0.0f, 0.0f);
		shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[0].constant", 1.0f);
		shader.setFloat("pointLights[0].linear", 0.09);
		shader.setFloat("pointLights[0].quadratic", 0.032);

		ourModel.draw(shader);


		// 绘制点光源的正方体
		float r = 1.0f;
		lightPos.x = r * sin(glfwGetTime());
		lightPos.z = r * cos(glfwGetTime());
		lightPos.y = 1.0f;
		EASY::drawCube(lightPos, glm::vec3(0.1f, 0.1f, 0.1f));
	}

	EASY::terminate();	// 必须

	return 0;
}

