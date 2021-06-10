#include <string>
#include <iostream>
#include <stb_image.h>
#include <opengl/texture2d.h>

void Texture2D::loadFromMemory(const GLsizei width, const GLsizei height, const GLsizei mipmapLevels, const GLenum internalFormat, const GLenum format, const GLenum type, const void* data) const
{
	assert(m_handler);
	setTexFormat(mipmapLevels, internalFormat, width, height);
	setTexImageData(width, height, format, type, data);
	initialParameters(mipmapLevels > 1);
	if (mipmapLevels > 1) genTexMipMap();
}

void Texture2D::loadHDRFile(const std::string& filePath, unsigned int mipmapLevels) const
{
	assert(m_handler);
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	float* data = stbi_loadf(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (data == nullptr)
	{
		throw std::runtime_error("Texture2D: load hdr texture data failure");
	}
	GLenum internalFormat, format, type = GL_FLOAT;
	if (nrChannels == 1)
	{
		internalFormat = GL_R16F;
		format = GL_RED;
	}
	else if (nrChannels == 3)
	{
		internalFormat = GL_RGB16F;
		format = GL_RGB;
	}
	else
	{
		internalFormat = GL_RGBA16F;
		format = GL_RGBA;
	}
	if (mipmapLevels == Texture2D::MAX_MIPMAP)
	{
		mipmapLevels = static_cast<int>(std::log2((width < height ? width : height)));
	}
	setTexFormat(mipmapLevels, internalFormat, width, height);
	setTexImageData(width, height, format, type, data);
	setTexFilterParameter(GL_LINEAR, GL_LINEAR);
	setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	stbi_image_free(data);
}

void Texture2D::loadCommonFile(const std::string& filePath, unsigned int mipmapLevels) const
{
	int width, height, nrChannels;
	auto* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (data == nullptr)
	{
		throw std::runtime_error("Texture2D: load texture data failure");
	}
	GLenum internalFormat, format, type;
	type = GL_UNSIGNED_BYTE;
	if (nrChannels == 1)
	{
		internalFormat = GL_R8;
		format = GL_RED;
	}
	else if (nrChannels == 3)
	{
		internalFormat = GL_RGB8;
		format = GL_RGB;
	}
	else
	{
		internalFormat = GL_RGBA8;
		format = GL_RGBA;
	}
	if (mipmapLevels == Texture2D::MAX_MIPMAP)
	{
		mipmapLevels = static_cast<int>(std::log2((width < height ? width : height)));
	}
	loadFromMemory(width, height, mipmapLevels, internalFormat, format, type, data);
	stbi_image_free(data);
}


void Texture2D::loadFromFile(const std::string& filePath, unsigned int mipmapLevels) const
{
	assert(m_handler);
	size_t k = filePath.find_last_of('.');
	std::string fileFormat = filePath.substr(k+1);
	if (fileFormat == "hdr")
	{
		loadHDRFile(filePath, mipmapLevels);
	}
	else
	{
		loadCommonFile(filePath, mipmapLevels);
	}
}

