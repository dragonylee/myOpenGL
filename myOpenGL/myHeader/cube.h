#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"


const float cube_vertices[] = {
	// positions
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
};

class Cube
{
public:
	// ���캯��
	Cube()
	{
		setupCube();
	}

	void draw(Shader& shader)
	{
		shader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}


private:
	unsigned int VAO, VBO;

	void setupCube()
	{
		/*
			���Զ�֮���ǣ�ͨ��һ��VAO��vertex array��ȥ����VBO(vertex buffer)�Լ��������ԣ�vertex attribute����
			����VBO��ʾ���㻺�壬���ڴ洢�����λ�õ���Ϣ����������������������ν���VBO��
			ͨ��һ��VAO��Ӧһ��VBO���Լ����vertex attribute���ڽ���VBO
			��EBO�����������У����е�VAO�ᱣ��EBO�İ�״̬
		*/
		// ����һ��VAO�Ļ�������
		// --------------------------------------
		// 1. ����VAO��VBO��EBO���Ǳ�Ҫ������
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		//glGenBuffers(1, &EBO);
		// 2. ��VAO
		glBindVertexArray(VAO);
		// 3. ����VBO���趨��������
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
		// (�Ǳ�Ҫ������EBO���趨��������
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// 4. ���Ӷ�������
		// glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
		// index: Ҫ���õĶ������Եı��
		// size: �������ԵĴ�С��vec3����3
		// type: ��������
		// normalized: �Ƿ��׼��
		// stride: ����
		// pointer: ƫ����
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// 5. ������黺��İ�
		// glVertexAttribPointer��VBOע��Ϊ�������԰󶨵Ķ��㻺�壬��˿��Խ����
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// ��VAO���ڼ���״̬ʱ�����ܽ��EBO�İ󶨣���ΪVAO��洢glBindBuffer�ĺ������ã���VAO��Ҫֱ�Ӱ�EBO
		// 6. ���VAO�İ󶨡����Ǳ�Ҫ�����EBO�İ�
		// ��5��6�����Ǳ�Ҫ�ģ���Ϊ��������VAO��VBO��ʱ��ͻ��Զ�������
		glBindVertexArray(0);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//--------------------------------------------
	}
};
