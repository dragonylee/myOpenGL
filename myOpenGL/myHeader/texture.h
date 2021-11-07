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
	// 不需要纹理类型的时候，定义为NONE
	NONE,
};

class Texture
{
private:
	bool flag = true;
public:
	unsigned int id = 0;	// 纹理编号
	TextureType type;		// 纹理类型
	std::string path;		// 纹理数据的路径

	// 构造函数
	Texture() {}
	Texture(const char* image_path, TextureType type, bool flipY = false);
	Texture(std::string image_path, TextureType type, bool flipY = false);
	Texture(const aiTexture* aiTex, TextureType type, bool flipY = false);

	// 获取纹理
	// 如果之前加载过，直接读取返回；否则进行纹理生成然后返回；
	static Texture getTexture(std::string texturePath, TextureType type = NONE, bool flipY = false);

	// 指定在第几个纹理单元用这个纹理
	// textureIndex可以是0,1,2,...，也可以是GL_TEXTURE0等
	void use(int textureIndex);

private:
	void load_texture(bool flipY, unsigned char* data, int width, int height, int nrChannels);
	unsigned char* load_data(const char* image_path, int* width, int* height, int* nrChannels);
	unsigned char* load_data(const aiTexture* aiTex, int* width, int* height, int* nrChannels);

	void add();
};


// [PBUG] 这里的键是texture的path，应当改为path+type会更好一些
// 用于保存已加载的纹理
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

	// 生成一个纹理的基本过程
	// --------------------------------------------
	// 1. 生成一个纹理编号
	glGenTextures(1, &id);
	// 2. 绑定纹理
	glBindTexture(GL_TEXTURE_2D, id);
	// 3. 设置纹理环绕方式（纹理比图像小的时候怎么处理，默认为重复）
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

	// 4. 设置纹理过滤方式 Texture Filtering
	//		(a)邻近过滤(GL_NEAREST)：选取最近的像素点
	//		(b)线性过滤(GL+LINEAR)：基于附近的像素颜色计算一个插值
	// 在进行放大和缩小操作的时候可以设置纹理过滤选项
	//		(c)多级渐远纹理 Mipmap
	//			在图形缩放到很小的时候，为了避免失真（不知道如何获取正确的颜色值），使用Mipmap去适配最合适的那个
	//			只在缩小的时候才可以使用Mipmap
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 5. 生成纹理，然后释放图像内存
	if (flipY) stbi_set_flip_vertically_on_load(true);

	GLenum format = GL_RGB;
	if (nrChannels == 1) format = GL_RED;
	else if (nrChannels == 3) format = GL_RGB;
	else if (nrChannels == 4) format = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);	// 生成多级渐远纹理图像

	if (flipY) stbi_set_flip_vertically_on_load(false);
	stbi_image_free(data);
	// 6. 解除绑定
	glBindTexture(GL_TEXTURE_2D, 0);
	// --------------------------------------------
}

unsigned char* Texture::load_data(const char* image_path, int* width, int* height, int* nrChannels)
{
	// 保存路径
	this->path = std::string(image_path);

	return stbi_load(image_path, width, height, nrChannels, 0);
}

unsigned char* Texture::load_data(const aiTexture* aiTex, int* width, int* height, int* nrChannels)
{
	// 保存路径
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