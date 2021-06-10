#pragma once

#include <glad/glad.h>

#include <cassert>

#include "buffer.h"
#include "vertex_attrib.h"
#include "vertex_buffer.h"

class VertexArray
{
public:
	VertexArray();

	VertexArray(const VertexArray& vao) = delete;

	VertexArray(VertexArray&& vao) noexcept;

	VertexArray& operator=(const VertexArray& vao) = delete;

	VertexArray& operator=(VertexArray&& vao) noexcept;

	~VertexArray();

	void reInitHandler();

	void destroy();

	void bindElementBuffer(const Buffer& buffer) const;

	template<typename Vertex>
	void bindVertexBuffer(const GLuint bindingIndex, const VertexBuffer<Vertex>& vbo,
	                      const size_t vertex_offsets) const;

	void bindVertexArrayAttrib(const GLuint bindingIndex, const VertexAttrib& vertexAttrib,
	                           const bool normalized = GL_FALSE) const;

	void enableVertexAtrrib(const VertexAttrib& vertexAttrib) const;

	void bindVertexAttrib(const VertexAttrib& vertexAttrib, const GLuint bindingIndex) const;

	void setVertexAtrribFormat(const VertexAttrib& vertexAttrib, const bool normalized) const;

	void bind() const;

	void unbind() const;

	unsigned int getHandler() const;

private:
	GLuint m_handler;
};

inline VertexArray::VertexArray():m_handler(0)
{
	glCreateVertexArrays(1, &m_handler);
}

template <typename Vertex>
void VertexArray::bindVertexBuffer(const GLuint bindingIndex, const VertexBuffer<Vertex>& vbo,
                                   const size_t vertex_offsets) const
{
	assert(m_handler);
	glVertexArrayVertexBuffer(m_handler, bindingIndex, vbo.getHandle(), sizeof(Vertex) * vertex_offsets,
	                          sizeof(Vertex));
}

inline VertexArray::VertexArray(VertexArray&& vao) noexcept: m_handler(vao.getHandler())
{
	vao.m_handler = 0;
}

inline VertexArray& VertexArray::operator=(VertexArray&& vao) noexcept
{
	destroy();
	m_handler = vao.getHandler();
	vao.m_handler = 0;
	return *this;
}

inline VertexArray::~VertexArray()
{
	destroy();
}

inline void VertexArray::reInitHandler()
{
	destroy();
	glCreateVertexArrays(1, &m_handler);
}

inline void VertexArray::destroy()
{
	if (m_handler)
	{
		glDeleteVertexArrays(1, &m_handler);
		m_handler = 0;
	}
}

inline void VertexArray::bindElementBuffer(const Buffer& buffer) const
{
	assert(m_handler);
	glVertexArrayElementBuffer(m_handler, buffer.getHandle());
}

inline void VertexArray::bindVertexArrayAttrib(const GLuint bindingIndex, const VertexAttrib& vertexAttrib,
                                               const bool normalized) const
{
	assert(m_handler);
	enableVertexAtrrib(vertexAttrib);
	bindVertexAttrib(vertexAttrib, bindingIndex);
	setVertexAtrribFormat(vertexAttrib, normalized);
}

inline void VertexArray::enableVertexAtrrib(const VertexAttrib& vertexAttrib) const
{
	assert(m_handler);
	glEnableVertexArrayAttrib(m_handler, vertexAttrib.attribIndex);
}

inline void VertexArray::bindVertexAttrib(const VertexAttrib& vertexAttrib, const GLuint bindingIndex) const
{
	assert(m_handler);
	glVertexArrayAttribBinding(m_handler, vertexAttrib.attribIndex, bindingIndex);
}

inline void VertexArray::setVertexAtrribFormat(const VertexAttrib& vertexAttrib, const bool normalized) const
{
	assert(m_handler);
	glVertexArrayAttribFormat(m_handler, vertexAttrib.attribIndex, vertexAttrib.size, vertexAttrib.type, normalized,
	                          vertexAttrib.relativeOffset);
}

inline void VertexArray::bind() const
{
	assert(m_handler);
	glBindVertexArray(m_handler);
}

inline void VertexArray::unbind() const
{
	assert(m_handler);
	glBindVertexArray(0);
}

inline unsigned VertexArray::getHandler() const
{
	return m_handler;
}
