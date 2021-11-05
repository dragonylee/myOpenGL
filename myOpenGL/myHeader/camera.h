#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement
{
	FORWARD,
	BACKWORD,
	LEFT,
	RIGHT,
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 12.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


/*
* 使用Camera的基本方法：
* 定义一个Camera实例，然后按照需要分别注册按键、鼠标移动、滚轮事件，
* 在事件中分别调用Camera.processKeyBoard, Camera.processMouseMovement, Camera.processMouseScroll。
*
* 调用 Camera.getViewMatrix() 获取从 world space 到 view space 的变换矩阵
*/

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;		// 偏航角
	float pitch;	// 俯仰角

	float movementSpeed;		// 相机移动速度
	float mouseSensitivity;		// 鼠标灵敏度
	float zoom;					// 放缩

	// 构造函数
	Camera(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW,
		float pitch = PITCH
	)
	{
		this->front = glm::vec3(0.0f, 0.0f, -1.0f);
		this->movementSpeed = SPEED;
		this->mouseSensitivity = SENSITIVITY;
		this->zoom = ZOOM;

		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		updateCameraVectors();
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw = YAW, float pitch = PITCH)
	{
		this->front = glm::vec3(0.0f, 0.0f, -1.0f);
		this->movementSpeed = SPEED;
		this->mouseSensitivity = SENSITIVITY;
		this->zoom = ZOOM;

		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(position, position + front, up);
	}

	void processKeyBoard(CameraMovement direction, float deltaTime)
	{
		float velocity = movementSpeed * deltaTime;
		if (direction == CameraMovement::FORWARD) position += front * velocity;
		if (direction == CameraMovement::BACKWORD) position -= front * velocity;
		if (direction == CameraMovement::LEFT) position -= right * velocity;
		if (direction == CameraMovement::RIGHT) position += right * velocity;
	}

	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		yaw += xoffset * mouseSensitivity;
		pitch -= yoffset * mouseSensitivity;
		if (constrainPitch)
		{
			if (pitch > 89.0f) pitch = 89.0f;
			if (pitch < -89.0f) pitch = -89.0f;
		}
		updateCameraVectors();
	}

	void processMouseScroll(float yoffset)
	{
		zoom -= yoffset;
		if (zoom < 1.0f) zoom = 1.0f;
		if (zoom > 45.0f) zoom = 45.0f;
	}

private:
	void updateCameraVectors()
	{
		glm::vec3 front = glm::vec3(
			cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
			sin(glm::radians(pitch)),
			sin(glm::radians(yaw)) * cos(glm::radians(pitch))
		);
		this->front = glm::normalize(front);
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}
};
