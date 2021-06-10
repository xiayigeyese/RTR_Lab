#pragma once

#include <cassert>
#include <glad/glad.h>

#include "texture2d.h"
#include "renderbuffer.h"

class Framebuffer
{
public:
	Framebuffer() :m_handler(0) 
	{
		glCreateFramebuffers(1, &m_handler);
	}

	Framebuffer(const Framebuffer&) = delete;

	Framebuffer(Framebuffer&& framebuffer) noexcept
		:m_handler(framebuffer.m_handler)
	{
		framebuffer.m_handler = 0;
	}

	Framebuffer& operator=(const Framebuffer&) = delete;

	Framebuffer& operator=(Framebuffer&& framebuffer) noexcept
	{
		destroy();
		m_handler = framebuffer.m_handler;
		framebuffer.m_handler = 0;
		return *this;
	}

	~Framebuffer()
	{
		destroy();
	}

	void destroy() 
	{
		if (m_handler) {
			glDeleteFramebuffers(1, &m_handler);
			m_handler = 0;
		}
	}

	void reInitHandler()
	{
		destroy();
		glCreateFramebuffers(1, &m_handler);
	}

	// check complete, and bind framebuffer, then use it
	void bind()
	{
		assert(m_handler);
		glBindFramebuffer(GL_FRAMEBUFFER, m_handler);
	}

	void unbind() 
	{
		assert(m_handler);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void attachTexture2D(const GLenum attachment, const Texture2D& texture, int level = 0 ) const
	{
		assert(m_handler);
		glNamedFramebufferTexture(m_handler, attachment, texture.getHandler(), level);
	}

	// use this api: need to attach a geometry shader that duplicates the primitive for each face
	void attachCubeMap(const GLenum attachment, const CubeMap& cubeMap) const
	{
		assert(m_handler);
		glNamedFramebufferTexture(m_handler, attachment, cubeMap.getHandler(), 0);
	}
	// use this api: need to attach and draw for each face for cubeMap
	void attachCubeMapFace(const GLenum attachment, const CubeMap& cubeMap, unsigned int layer, unsigned int face) const
	{
		assert(m_handler);
		glNamedFramebufferTextureLayer(m_handler, attachment, cubeMap.getHandler(), layer, face);
	}

	void attachRenderBuffer(const GLenum attachment, const Renderbuffer& renderbuffer) const
	{
		assert(m_handler);
		glNamedFramebufferRenderbuffer(m_handler, attachment, GL_RENDERBUFFER, renderbuffer.getHandler());
	}

	void setColorBufferToDraw(GLenum buffer) const
	{
		assert(m_handler);
		glNamedFramebufferDrawBuffer(m_handler, buffer);
	}

	void setColorBuffersToDraw(int n, GLenum* buffer) const
	{
		assert(m_handler);
		glNamedFramebufferDrawBuffers(m_handler, n, buffer);
	}

	void setColorBufferToRead(GLenum buffer) const
	{
		assert(m_handler);
		glNamedFramebufferReadBuffer(m_handler, buffer);
	}

	[[nodiscard]] bool isComplete() const
	{
		assert(m_handler);
		return glCheckNamedFramebufferStatus(m_handler, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}

	[[nodiscard]] GLuint getHandler() const
	{
		return m_handler;
	}


private:
	GLuint m_handler;
};