#pragma once

#include <utility>

#include "buffer.h"

template<typename T>
class VertexBuffer : public Buffer
{
public:
	VertexBuffer();

	VertexBuffer(const T* data, const size_t vertex_count, const GLenum usage = GL_STATIC_DRAW);

	VertexBuffer(VertexBuffer& vbo) = delete;

	VertexBuffer(VertexBuffer&& vbo) noexcept;

	VertexBuffer& operator=(VertexBuffer& vbo) = delete;

	VertexBuffer& operator=(VertexBuffer&& vbo) noexcept;

	~VertexBuffer();

	void reInitHandler();

	void setData(const T* data, const size_t vertex_count, const GLenum usage = GL_STATIC_DRAW);

	void setSubData(const T* subData, const size_t vertex_count, const size_t vertex_offset);

	void destroy();

	size_t getVertexCount() const;

private:
	size_t m_vertexCount;
};

template<typename T>
VertexBuffer<T>::VertexBuffer() : Buffer(), m_vertexCount(0)
{
}

template <typename T>
VertexBuffer<T>::VertexBuffer(const T* data, const size_t vertex_count, const GLenum usage): Buffer()
{
	this->setData(data, vertex_count, usage);
}

template <typename T>
VertexBuffer<T>::VertexBuffer(VertexBuffer&& vbo) noexcept: Buffer(std::move(static_cast<Buffer&>(vbo))),
                                                            m_vertexCount(vbo.getVertexCount())
{
	vbo.m_vertexCount = 0;
}

template <typename T>
VertexBuffer<T>& VertexBuffer<T>::operator=(VertexBuffer&& vbo) noexcept
{
	Buffer::operator=(std::move(static_cast<Buffer&>(vbo)));
	m_vertexCount = vbo.getVertexCount();
	vbo.m_vertexCount = 0;
	return *this;
}

template <typename T>
VertexBuffer<T>::~VertexBuffer()
{
	destroy();
}

template <typename T>
void VertexBuffer<T>::reInitHandler()
{
	Buffer::reInitHandler();
}

template <typename T>
void VertexBuffer<T>::setData(const T* data, const size_t vertex_count, const GLenum usage)
{
	assert(vertex_count);
	Buffer::setData(data, sizeof(T) * vertex_count, usage);
	m_vertexCount = vertex_count;
}

template <typename T>
void VertexBuffer<T>::setSubData(const T* subData, const size_t vertex_count, const size_t vertex_offset)
{
	assert(vertex_count);
	Buffer::setSubData(subData, sizeof(T) * vertex_count, sizeof(T) * vertex_offset);
}

template <typename T>
void VertexBuffer<T>::destroy()
{
	Buffer::destroy();
	m_vertexCount = 0;
}

template <typename T>
size_t VertexBuffer<T>::getVertexCount() const
{
	return m_vertexCount;
}
