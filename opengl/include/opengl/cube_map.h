#pragma once

#include <utility>
#include <array>
#include <iostream>

#include "texture.h"

class CubeMap: public Texture
{

public:
	CubeMap()
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_handler);
	}

	CubeMap(const CubeMap&) = delete;

	CubeMap(CubeMap&& cubeMap) noexcept
		:Texture(std::move(static_cast<Texture&>(cubeMap)))
	{
	}

	CubeMap& operator=(const CubeMap&) = delete;

	CubeMap& operator=(CubeMap&& cubeMap) noexcept
	{
		Texture::operator=(std::move(static_cast<Texture&>(cubeMap)));
		return *this;
	}

	~CubeMap()
	{
		destroy();
	}

	void reInitHandler() 
	{
		destroy();
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_handler);
	}

	void setTexWrapParameter(const GLint wrapSParam, const GLint wrapTParam, const GLint wrapRParam) const
	{
		assert(m_handler);
		glTextureParameteri(m_handler, GL_TEXTURE_WRAP_S, wrapSParam);
		glTextureParameteri(m_handler, GL_TEXTURE_WRAP_T, wrapTParam);
		glTextureParameteri(m_handler, GL_TEXTURE_WRAP_R, wrapRParam);
	}

	void setTexImageData(GLsizei width, GLsizei height, GLenum format, GLenum type, const std::array<void *, 6>& data) const
	{
		assert(m_handler);
		for(int face=0;face<6; face++)
		{
			glTextureSubImage3D(m_handler, 
								0, 
								0, 0, face, 
								width, height, 1, 
								format, 
								type, 
								data[face]);
		}
	}

	void loadFromFiles(const std::array<std::string, 6>& filePaths,
		const GLenum internalFormat,
		const GLenum format,
		const GLenum type) const;
};