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
* ʹ��Camera�Ļ���������
* ����һ��Cameraʵ����Ȼ������Ҫ�ֱ�ע�ᰴ��������ƶ��������¼���
* ���¼��зֱ����Camera.processKeyBoard, Camera.processMouseMovement, Camera.processMouseScroll��
*
* ���� Camera.getViewMatrix() ��ȡ�� world space �� view space �ı任����
*/

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;		// ƫ����
	float pitch;	// ������

	float movementSpeed;		// ����ƶ��ٶ�
	float mouseSensitivity;		// ���������
	float zoom;					// ����

	// ���캯��
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
