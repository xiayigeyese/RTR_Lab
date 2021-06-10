#pragma once

#include <thread>

#include <cassert>
#include <glad/glad.h>

class Renderbuffer 
{
public:
	Renderbuffer() :m_handler(0)
	{
		glCreateRenderbuffers(1, &m_handler);
	}

	Renderbuffer(const Renderbuffer&) = delete;
	Renderbuffer(Renderbuffer&& renderbuffer) noexcept
		:m_handler(renderbuffer.m_handler)
	{
		renderbuffer.m_handler = 0;
	}

	Renderbuffer& operator=(const Renderbuffer&) = delete;
	Renderbuffer& operator=(Renderbuffer&& renderbuffer) noexcept
	{
		m_handler = renderbuffer.m_handler;
		renderbuffer.m_handler = 0;
		return *this;
	}

	~Renderbuffer()
	{
		destroy();
	}

	void destroy()
	{
		if (m_handler)
		{
			glDeleteRenderbuffers(1, &m_handler);
			m_handler = 0;
		}
	}

	void reInitHandler()
	{
		destroy();
		glCreateRenderbuffers(1, &m_handler);
	}

	void allocateStorage(const GLenum format, const unsigned int width, const unsigned int height) const
	{
		assert(m_handler);
		glNamedRenderbufferStorage(m_handler, format, width, height);
	}

	[[nodiscard]] GLuint getHandler() const 
	{
		return m_handler;
	}

private:
	GLuint m_handler;
};