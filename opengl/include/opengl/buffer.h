#pragma once

#include <cassert>

#include <glad/glad.h>

class Buffer
{
public:
	Buffer();
	
	Buffer(const Buffer& b) = delete;

	Buffer& operator =(const Buffer& b) = delete;
	                                                                  
	Buffer(Buffer&& b) noexcept;

	Buffer& operator=(Buffer&& b) noexcept;

	~Buffer();

	void reInitHandler();

	void setData(const void* p, const GLsizeiptr data_size, const GLenum usage = GL_STATIC_DRAW) const;

	void setSubData(const void* data, const GLsizeiptr data_size, const GLintptr offset) const;

	void destroy();

	[[nodiscard]] GLuint getHandle() const;

protected:
	GLuint m_handler;
};

inline Buffer::Buffer():m_handler(0)
{
	glCreateBuffers(1, &m_handler);
}

inline Buffer::Buffer(Buffer&& b) noexcept: m_handler(b.m_handler)
{
	b.m_handler = 0;
}

inline Buffer& Buffer::operator=(Buffer&& b) noexcept
{
	destroy();
	m_handler = b.m_handler;
	b.m_handler = 0;
	return *this;
}

inline Buffer::~Buffer()
{
	destroy();
}

inline void Buffer::reInitHandler()
{
	destroy();
	glCreateBuffers(1, &m_handler);
}

inline void Buffer::setData(const void* data, const GLsizeiptr data_size, const GLenum usage) const
{
	assert(m_handler);
	glNamedBufferData(m_handler, data_size, data, usage);
}

inline void Buffer::setSubData(const void* data, const GLsizeiptr data_size, const GLintptr offset) const
{
	assert(m_handler);
	glNamedBufferSubData(m_handler, offset, data_size, data);
}

inline void Buffer::destroy()
{
	if (m_handler)
	{
		glDeleteBuffers(1, &m_handler);
		m_handler = 0;
	}
}

inline GLuint Buffer::getHandle() const
{
	return m_handler;
}
