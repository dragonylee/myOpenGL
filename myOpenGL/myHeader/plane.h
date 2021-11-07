#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

// ƽ�� ��������Ϊ���ĵ�

/*
* ������������ƽ�����ͣ�������
*	- ������ƽ��	PURE_PLANE		��VBO��ÿ����λ����3��float��ʾ��������
*	- ����ƽ��		TEXTURE_PLANE	��VBO��ÿ����λ����5��float��ʾ ��������3+��������2
*/

enum PlaneType
{
	PURE_PLANE,
	TEXTURE_PLANE,
};


//float texture_plane_vertices[] = {
//	// positions          // texture Coords
//	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
//	-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
//	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
//
//	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
//	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
//	 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
//};

float texture_plane_vertices[] = {
	// positions          // texture Coords
	 1.0f, 0.0f,  1.0f,  1.0f, 0.0f,
	-1.0f, 0.0f,  1.0f,  0.0f, 0.0f,
	-1.0f, 0.0f, -1.0f,  0.0f, 1.0f,

	 1.0f, 0.0f,  1.0f,  1.0f, 0.0f,
	-1.0f, 0.0f, -1.0f,  0.0f, 1.0f,
	 1.0f, 0.0f, -1.0f,  1.0f, 1.0f
};

class Plane
{
private:
	PlaneType type;
public:
	// ���캯��
	Plane(PlaneType type) :type(type)
	{
		setupPlane();
	}

	void draw(Shader& shader)
	{
		shader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}


private:
	unsigned int VAO, VBO;

	void setupPlane()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (type == PlaneType::TEXTURE_PLANE)
			glBufferData(GL_ARRAY_BUFFER, sizeof(texture_plane_vertices), texture_plane_vertices, GL_STATIC_DRAW);

		if (type == PlaneType::TEXTURE_PLANE)
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
		}

		glBindVertexArray(0);
	}
};
