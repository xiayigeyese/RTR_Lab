#pragma once

#include <utility>
#include <string>

#include "texture.h"

class Texture2D: public Texture
{
public:
	static constexpr unsigned int MAX_MIPMAP = 0;
	Texture2D()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_handler);
	}

	Texture2D(const Texture2D&) = delete;

	Texture2D(Texture2D&& texture) noexcept
		:Texture(std::move(static_cast<Texture&>(texture)))
	{
	}

	Texture2D& operator=(const Texture2D&) = delete;

	Texture2D& operator=(Texture2D&& texture) noexcept
	{
		Texture::operator=(std::move(static_cast<Texture&>(texture)));
		return *this;
	}
	
	~Texture2D()
	{
		destroy();
	}
	
	void reInitHandler()
	{
		destroy();
		glCreateTextures(GL_TEXTURE_2D, 1, &m_handler);
	}

	void setTexWrapParameter(const GLint wrapSParam, const GLint wrapTParam) const
	{
		assert(m_handler);
		glTextureParameteri(m_handler, GL_TEXTURE_WRAP_S, wrapSParam);
		glTextureParameteri(m_handler, GL_TEXTURE_WRAP_T, wrapTParam);
	}
	
	void setTexImageData(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data) const 
	{
		assert(m_handler);
		glTextureSubImage2D(m_handler, 0, 0, 0, width, height, format, type, data);
	}
	
	void initialParameters(bool genMipMap = true) const
	{
		assert(m_handler);
		setTexWrapParameter(GL_REPEAT, GL_REPEAT);
		if (genMipMap)
		{
			setTexFilterParameter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		}
		else
		{
			setTexFilterParameter(GL_LINEAR, GL_LINEAR);
		}
	}
   
	void loadFromMemory(const GLsizei width,
		const GLsizei height,
		const GLsizei levels,
		const GLenum internalFormat,
		const GLenum format,
		const GLenum type,
		const void* data
	) const;

	void loadFromFile(const std::string& filePath, unsigned int mipmapLevels = 1) const;

private:
	void loadHDRFile(const std::string& filePath, unsigned int mipmapLevels) const;

	void loadCommonFile(const std::string& filePath, unsigned int mipmapLevels) const;
};