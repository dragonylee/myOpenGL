#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"
#include "texture.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>


class Model
{
public:
	std::vector<Texture> texturesLoaded;	//
	std::vector<Mesh> meshes;
	std::string directory;					// ģ��·��
	bool gammaCorrection;
	bool isPrintInfo;

	// ���캯��
	Model(const std::string& path, bool printInfo = false, bool gamma = false) :isPrintInfo(printInfo), gammaCorrection(gamma)
	{
		loadModel(path);
	}

	// ����Model
	void draw(Shader& shader)
	{
		// ����ÿһ��mesh
		for (int i = 0; i < meshes.size(); i++)
			meshes[i].draw(shader);
	}

private:
	// ��ӡһЩ��Ϣ
	void printInfo(const aiScene* scene)
	{
		std::cout << "---------------------------- INFO -------------------------" << std::endl;

		std::cout << "Mesh number:  " << scene->mNumMeshes << std::endl;
		std::cout << "Materials number:  " << scene->mNumMaterials << std::endl;

		std::cout << "-----------------------------------------------------------" << std::endl;
	}

	// ����ģ��
	void loadModel(const std::string& path)
	{
		Assimp::Importer importer;
		// scene�а������������ݣ��������node�Ȱ�����������Ϣ
		const aiScene* scene = importer.ReadFile(
			path,
			// aiProcess_Triangulate ��ͼԪ��״���������
			// aiProcess_GenSmoothNormals ���û�з�����������������
			// aiProcess_FlipUVs ��תy����������
			// aiProcess_CalcTangentSpace �������ߺ͸�����
			// ����鿴 http://assimp.sourceforge.net/lib_html/postprocess_8h.html
			aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
		);
		// ����Ƿ�ɹ�����
		if (!scene || (scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		// ����Ŀ¼
		directory = path.substr(0, path.find_last_of('/'));
		// �ж��Ƿ��ӡ��Ϣ
		if (isPrintInfo)
		{
			printInfo(scene);
		}
		// ���ú����ݹ鴦��node
		processNode(scene->mRootNode, scene);
	}

	// �ݹ鴦������node
	// ������Ա������ӽڵ���Ϣ
	void processNode(aiNode* node, const aiScene* scene)
	{
		// ����node���������mesh
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// Ȼ��ݹ鴦�����ж��ӽڵ㣬����������һ���������
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	// ������mesh
	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// ����mesh��Ҫ����������
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		// ����mesh������vertex�����㣩����ȡvertices
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			// ����vertex
			Vertex vertex;
			// vertex.position
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;
			// vertex.normal
			if (mesh->HasNormals())
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}
			// vertex.texCoords
			if (mesh->HasTextureCoords(0))
			{
				// ����Ĭ��ֻʹ���˱��Ϊ0���������� [PBUG]
				vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
			}
			else
				vertex.texCoords = glm::vec2(0, 0);
			// vertex.tangent, vertex.bitangent
			if (mesh->HasTangentsAndBitangents())
			{
				vertex.tangent.x = mesh->mTangents[i].x;
				vertex.tangent.y = mesh->mTangents[i].y;
				vertex.tangent.z = mesh->mTangents[i].z;

				vertex.bitangent.x = mesh->mBitangents[i].x;
				vertex.bitangent.y = mesh->mBitangents[i].y;
				vertex.bitangent.z = mesh->mBitangents[i].z;
			}

			vertices.push_back(vertex);
		}

		// ����mesh������face��ͼԪ��������ȡindices
		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// ��ÿ��face�Ķ����������򱣴�
			for (int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		// ���������Ϣ��һ��meshֻ����һ��material������ж��Assimp���Զ��ָ�ɶ��mesh
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// Լ��shader��������������淶Ϊ��
		// "texture_" + ("diffuse"/"specular"/"normal"/"height") + to_string(N),   N = 1, 2, ...

		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE, scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, TextureType::SPECULAR, scene);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTexture(material, aiTextureType_NORMALS, TextureType::NORMAL, scene);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTexture(material, aiTextureType_HEIGHT, TextureType::HEIGHT, scene);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		// return
		return Mesh(vertices, indices, textures);
	}

	// ���ز�����������и������͵�����
	std::vector<Texture> loadMaterialTexture(aiMaterial* material, aiTextureType type, TextureType textureType, const aiScene* scene)
	{
		std::vector<Texture> textures;

		// ����ÿ����һ��texture��Ҫ����һ�����м��ع���texture�����Ż� [TODO]
		for (int i = 0; i < material->GetTextureCount(type); i++)
		{
			// ��ȡ�������·��
			aiString str;
			material->GetTexture(type, i, &str);
			// filename��¼�����·���������·����ģ���ļ�·��+�������·��
			std::string filename(str.C_Str());
			filename = directory + '/' + filename;

			// ����Ϊ����������һ���Ż�����texturesLoaded�����������Ѿ������˵�texture
			// ���������ǰ�����Ѿ����ع�����ֱ���ü��ɣ���������������
			bool flag = false;
			for (int j = 0; j < texturesLoaded.size(); j++)
			{
				if (texturesLoaded[j].path == filename)
				{
					textures.push_back(texturesLoaded[j]);
					flag = true;
					break;
				}
			}
			if (!flag)
			{
				// ��������
				const aiTexture* aitexture = scene->GetEmbeddedTexture(filename.c_str());
				if (aitexture)
				{
					Texture texture(aitexture, textureType);
					textures.push_back(texture);
					texturesLoaded.push_back(texture);
				}
				else
				{
					Texture texture(filename, textureType);
					textures.push_back(texture);
					texturesLoaded.push_back(texture);
				}
			}
		}

		return textures;
	}
};

