#pragma once

#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>

enum TextureType
{
	DIFFUSE,
	SPECULAR,
	NORMAL,
	HEIGHT,
	// ����Ҫ�������͵�ʱ�򣬶���ΪNONE
	NONE,
};

class Texture
{
private:
	bool flag = true;
public:
	unsigned int id = 0;	// ������
	TextureType type;		// ��������
	std::string path;		// �������ݵ�·��

	// ���캯��
	Texture() {}
	Texture(const char* image_path, TextureType type, bool flipY = false);
	Texture(std::string image_path, TextureType type, bool flipY = false);
	Texture(const aiTexture* aiTex, TextureType type, bool flipY = false);

	// ��ȡ����
	// ���֮ǰ���ع���ֱ�Ӷ�ȡ���أ����������������Ȼ�󷵻أ�
	static Texture getTexture(std::string texturePath, TextureType type = NONE, bool flipY = false);

	// ָ���ڵڼ�������Ԫ���������
	// textureIndex������0,1,2,...��Ҳ������GL_TEXTURE0��
	void use(int textureIndex);

private:
	void load_texture(bool flipY, unsigned char* data, int width, int height, int nrChannels);
	unsigned char* load_data(const char* image_path, int* width, int* height, int* nrChannels);
	unsigned char* load_data(const aiTexture* aiTex, int* width, int* height, int* nrChannels);

	void add();
};


// [PBUG] ����ļ���texture��path��Ӧ����Ϊpath+type�����һЩ
// ���ڱ����Ѽ��ص�����
std::map<std::string, Texture> string_texture_map;


// implementation

Texture::Texture(const char* image_path, TextureType type, bool flipY)
{
	this->type = type;

	int width, height, nrChannels;
	unsigned char* data = load_data(image_path, &width, &height, &nrChannels);
	load_texture(flipY, data, width, height, nrChannels);

	if (flag) add();
}

Texture::Texture(std::string image_path, TextureType type, bool flipY)
{
	*this = Texture(image_path.c_str(), type, flipY);
}

Texture::Texture(const aiTexture* aiTex, TextureType type, bool flipY)
{
	this->type = type;

	int width, height, nrChannels;
	unsigned char* data = load_data(aiTex, &width, &height, &nrChannels);
	load_texture(flipY, data, width, height, nrChannels);

	if (flag) add();
}

void Texture::use(int textureIndex)
{
	int x;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &x);
	if (textureIndex < x) textureIndex += GL_TEXTURE0;

	glActiveTexture(textureIndex);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::load_texture(bool flipY, unsigned char* data, int width, int height, int nrChannels)
{
	if (!data)
	{
		std::cout << "load texture error:  " << std::endl;
		flag = false;
		return;
	}

	// ����һ������Ļ�������
	// --------------------------------------------
	// 1. ����һ��������
	glGenTextures(1, &id);
	// 2. ������
	glBindTexture(GL_TEXTURE_2D, id);
	// 3. ���������Ʒ�ʽ�������ͼ��С��ʱ����ô����Ĭ��Ϊ�ظ���
	if (nrChannels <= 3)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else if (nrChannels == 4)	// RGBA
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	//float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// 4. ����������˷�ʽ Texture Filtering
	//		(a)�ڽ�����(GL_NEAREST)��ѡȡ��������ص�
	//		(b)���Թ���(GL+LINEAR)�����ڸ�����������ɫ����һ����ֵ
	// �ڽ��зŴ����С������ʱ����������������ѡ��
	//		(c)�༶��Զ���� Mipmap
	//			��ͼ�����ŵ���С��ʱ��Ϊ�˱���ʧ�棨��֪����λ�ȡ��ȷ����ɫֵ����ʹ��Mipmapȥ��������ʵ��Ǹ�
	//			ֻ����С��ʱ��ſ���ʹ��Mipmap
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 5. ��������Ȼ���ͷ�ͼ���ڴ�
	if (flipY) stbi_set_flip_vertically_on_load(true);

	GLenum format = GL_RGB;
	if (nrChannels == 1) format = GL_RED;
	else if (nrChannels == 3) format = GL_RGB;
	else if (nrChannels == 4) format = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);	// ���ɶ༶��Զ����ͼ��

	if (flipY) stbi_set_flip_vertically_on_load(false);
	stbi_image_free(data);
	// 6. �����
	glBindTexture(GL_TEXTURE_2D, 0);
	// --------------------------------------------
}

unsigned char* Texture::load_data(const char* image_path, int* width, int* height, int* nrChannels)
{
	// ����·��
	this->path = std::string(image_path);

	return stbi_load(image_path, width, height, nrChannels, 0);
}

unsigned char* Texture::load_data(const aiTexture* aiTex, int* width, int* height, int* nrChannels)
{
	// ����·��
	this->path = std::string(aiTex->mFilename.C_Str());

	unsigned int image_size = 1;
	if (aiTex->mHeight != 0) image_size *= aiTex->mHeight;
	if (aiTex->mWidth != 0) image_size *= aiTex->mWidth;

	return stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData), image_size, width, height, nrChannels, 0);
}


void Texture::add()
{
	if (string_texture_map.count(this->path) == 0)
	{
		string_texture_map[this->path] = *this;
	}
}

Texture Texture::getTexture(std::string texturePath, TextureType type, bool flipY)
{
	if (string_texture_map.count(texturePath))
	{
		return string_texture_map[texturePath];
	}

	std::cout << "load a new texture from " << texturePath << std::endl;

	return Texture(texturePath, type, flipY);
}