#pragma once

#include <array>
#include <vector>

#include <glad/glad.h>

#include "opengl/vertex_array.h"
#include "opengl/vertex_buffer.h"
#include "opengl/vertex_attrib.h"

class Quad
{
	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec2 texCoords;
	};
	
public:
	Quad()
	{
		initQuad();
	}

	void draw() const
	{
		m_vao.bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		m_vao.unbind();
	}

private:
	void initQuad()
	{
		std::array<QuadVertex, 4> quadVertices =
		{
			QuadVertex{ {-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			QuadVertex{ { -1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
			QuadVertex{ { 1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			QuadVertex{ { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
		};
		m_vbo.setData(quadVertices.data(), quadVertices.size());
		VertexAttrib positionAttrib = { 0, 3, GL_FLOAT, offsetof(QuadVertex, position) };
		VertexAttrib uvCoordsAttrib = { 1,2,GL_FLOAT, offsetof(QuadVertex, texCoords) };
		unsigned int bindingIndex = 0;
		m_vao.bindVertexBuffer(bindingIndex, m_vbo, 0);
		m_vao.bindVertexArrayAttrib(bindingIndex, positionAttrib, GL_FALSE);
		m_vao.bindVertexArrayAttrib(bindingIndex, uvCoordsAttrib, GL_FALSE);
	}

	VertexArray m_vao;
	VertexBuffer<QuadVertex> m_vbo;
};


class Cube
{
	struct CubeVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
	};
public:
	Cube()
	{
		initCube();
	}

	void draw() const
	{
		m_vao.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		m_vao.unbind();
	}

private:
	void initCube()
	{
		std::array<CubeVertex, 36> cubeVertices =
		{
			CubeVertex{{-1.0f, -1.0f, -1.0f},  {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, // bottom-left
			CubeVertex{{ 1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, // top-right
			CubeVertex{{ 1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}}, // bottom-right         
			CubeVertex{{ 1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, // top-right
			CubeVertex{{-1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, // bottom-left
			CubeVertex{{-1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}}, // top-left
			CubeVertex{{-1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // bottom-left
			CubeVertex{{ 1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // bottom-right
			CubeVertex{{ 1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // top-right
			CubeVertex{{ 1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // top-right
			CubeVertex{{-1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // top-left
			CubeVertex{{-1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // bottom-left
			CubeVertex{{-1.0f,  1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // top-right
			CubeVertex{{-1.0f,  1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // top-left
			CubeVertex{{-1.0f, -1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // bottom-left
			CubeVertex{{-1.0f, -1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // bottom-left
			CubeVertex{{-1.0f, -1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // bottom-right
			CubeVertex{{-1.0f,  1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // top-right
			CubeVertex{{ 1.0f,  1.0f,  1.0f}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // top-left
			CubeVertex{{ 1.0f, -1.0f, -1.0f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // bottom-right
			CubeVertex{{ 1.0f,  1.0f, -1.0f}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // top-right         
			CubeVertex{{ 1.0f, -1.0f, -1.0f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // bottom-right
			CubeVertex{{ 1.0f,  1.0f,  1.0f}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // top-left
			CubeVertex{{ 1.0f, -1.0f,  1.0f}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // bottom-left     
			CubeVertex{{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}}, // top-right
			CubeVertex{{ 1.0f, -1.0f, -1.0f}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}}, // top-left
			CubeVertex{{ 1.0f, -1.0f,  1.0f}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}, // bottom-left
			CubeVertex{{ 1.0f, -1.0f,  1.0f}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}, // bottom-left
			CubeVertex{{-1.0f, -1.0f,  1.0f}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}}, // bottom-right
			CubeVertex{{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}}, // top-right
			CubeVertex{{-1.0f,  1.0f, -1.0f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}, // top-left
			CubeVertex{{ 1.0f,  1.0f , 1.0f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, // bottom-right
			CubeVertex{{ 1.0f,  1.0f, -1.0f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, // top-right     
			CubeVertex{{ 1.0f,  1.0f,  1.0f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, // bottom-right
			CubeVertex{{-1.0f,  1.0f, -1.0f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}, // top-left
			CubeVertex{{-1.0f,  1.0f,  1.0f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}  // bottom-left 
		};
		m_vbo.setData(cubeVertices.data(), cubeVertices.size());
		VertexAttrib positionAttrib = { 0, 3, GL_FLOAT, offsetof(CubeVertex, position) };
		VertexAttrib normalAttrib = { 1, 3, GL_FLOAT, offsetof(CubeVertex, normal) };
		VertexAttrib uvCoordsAttrib = { 2, 2, GL_FLOAT, offsetof(CubeVertex, texCoords) };
		unsigned int bindingIndex = 0;
		m_vao.bindVertexBuffer(bindingIndex, m_vbo, 0);
		m_vao.bindVertexArrayAttrib(bindingIndex, positionAttrib, GL_FALSE);
		m_vao.bindVertexArrayAttrib(bindingIndex, normalAttrib, GL_FALSE);
		m_vao.bindVertexArrayAttrib(bindingIndex, uvCoordsAttrib, GL_FALSE);
	}

	VertexArray m_vao;
	VertexBuffer<CubeVertex> m_vbo;
};

class Sphere
{
	struct SphereVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
	};

public:
	Sphere(int segments = 64):m_indicesCount(0)
	{
		initSphere(segments);
	}

	void draw() const
	{
		m_vao.bind();
		glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, nullptr);
		m_vao.unbind();
	}

private:
	void initSphere(unsigned int segments)
	{
		unsigned int xSegments = segments, ySegments = segments;
		unsigned int totalCounts = (xSegments + 1) * (ySegments + 1);
		std::vector<SphereVertex> sphereVertices(totalCounts);
		for (unsigned int i = 0; i <= ySegments; i++)
		{
			for (unsigned int j = 0; j <= xSegments; j++)
			{
				// triangle vertex
				float u = static_cast<float>(j) / static_cast<float>(xSegments);
				float v = static_cast<float>(i) / static_cast<float>(ySegments);
				
				float theta = glm::radians(-90.0f + v * 180.0f);
				float phi = glm::radians(0.0f + u * 360.0f);

				float x = glm::cos(theta) * glm::cos(phi);
				float y = glm::sin(theta);
				float z = glm::cos(theta) * glm::sin(phi);
				
				glm::vec3 position = glm::vec3(x, y, z);
				glm::vec3 normal = glm::vec3(x, y, z);
				glm::vec2 texCoord = glm::vec2(u, v);
				sphereVertices[j + i * (xSegments + 1)] = { position, normal, texCoord };
			}
		}
		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < ySegments; i++)
		{
			for (unsigned int j = 0; j < xSegments; j++)
			{
				// triangle vertex index
				unsigned int index1, index2, index3;
				index1 = j + i * (xSegments + 1);
				index2 = j + (i + 1) * (xSegments + 1);
				index3 = index1 + 1;
				indices.push_back(index1);
				indices.push_back(index2);
				indices.push_back(index3);
				
				index1 = j + 1 + i * (xSegments + 1);
				index2 = j + (i + 1) * (xSegments + 1);
				index3 = j + (i + 1) * (xSegments + 1) + 1;
				indices.push_back(index1);
				indices.push_back(index2);
				indices.push_back(index3);
			}
		}
		m_vbo.setData(sphereVertices.data(), sphereVertices.size());
		VertexAttrib positionAttrib = { 0, 3, GL_FLOAT, offsetof(SphereVertex, position) };
		VertexAttrib normalAttrib = { 1, 3, GL_FLOAT, offsetof(SphereVertex, normal) };
		VertexAttrib texCoordAttrib = { 2, 2, GL_FLOAT, offsetof(SphereVertex, texCoords) };
		unsigned int bindingIndex = 0;
		m_vao.bindVertexBuffer(bindingIndex, m_vbo, 0);
		m_vao.bindVertexArrayAttrib(bindingIndex, positionAttrib);
		m_vao.bindVertexArrayAttrib(bindingIndex, normalAttrib);
		m_vao.bindVertexArrayAttrib(bindingIndex, texCoordAttrib);

		m_ebo.setData(indices.data(), indices.size() * sizeof(unsigned int));
		m_vao.bindElementBuffer(m_ebo);

		m_indicesCount = static_cast<unsigned int>(indices.size());
	}

	VertexArray m_vao;
	VertexBuffer<SphereVertex> m_vbo;
	Buffer m_ebo;
	unsigned int m_indicesCount;
};