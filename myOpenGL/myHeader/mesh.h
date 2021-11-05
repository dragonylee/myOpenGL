#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "shader.h"
#include "texture.h"

#define MAX_BONE_INFLUENCE 4

// ÿ�������Ϣ
struct Vertex
{
	glm::vec3 position;		// λ��
	glm::vec3 normal;		// ������
	glm::vec2 texCoords;	// ��������
	glm::vec3 tangent;		//
	glm::vec3 bitangent;	//
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

class Mesh
{
public:
	// ��������
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// ����
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		setupMesh();
	}

	void draw(Shader& shader)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		
		for (int i = 0; i < textures.size(); i++)
		{
			// ��ȡfragment shader�������ȷ����
			// Լ��shader��������������淶Ϊ��
			// "texture_" + ("diffuse"/"specular"/"normal"/"height") + to_string(N),   N = 1, 2, ...
			std::string number;
			std::string name = textureType2NameInShader(textures[i].type);
			if (name == "texture_diffuse") number = std::to_string(diffuseNr++);
			else if (name == "texture_specular") number = std::to_string(specularNr++);
			else if (name == "texture_normal") number = std::to_string(normalNr++);
			else if (name == "texture_height") number = std::to_string(heightNr++);
			// Ϊshader�����Ӧ���ֵĲ�����������ȷ��������
			// ע��fragment shader���������������material�ṹ������
			shader.setInt(("material." + name + number).c_str(), i);
			// ��texture
			textures[i].use(i);
		}
		// ����mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		// ��ԭ
		glActiveTexture(GL_TEXTURE0);
	}

private:
	unsigned int VAO, VBO, EBO;

	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// ����λ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		// ���㷨��
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);
		// ������������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
		glEnableVertexAttribArray(2);
		// tangent
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
		glEnableVertexAttribArray(3);
		// bitangent
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
		glEnableVertexAttribArray(4);
		// ids
		glVertexAttribPointer(5, MAX_BONE_INFLUENCE, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
		glEnableVertexAttribArray(2);
		// weights
		glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	std::string textureType2NameInShader(TextureType type)
	{
		if (type == TextureType::DIFFUSE) return "texture_diffuse";
		if (type == TextureType::HEIGHT) return "texture_height";
		if (type == TextureType::NORMAL) return "texture_normal";
		if (type == TextureType::SPECULAR) return "texture_specular";
	}
};
