#pragma once

#include <vector>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <opengl/opengl.h>

struct MeshVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};


enum class VERTEX_ATTRIBUTE
{
	P_N_UV,          // P: position, N: normal, UV: texCoords      
	P_N_UV_T_B       // T: tangent, B: bitangent
};


enum class TEXTURE_TYPE
{
	DIFFUSE,
	SPECULAR,
	NORMAL,
	HEIGHT
};

// one material in model use for many mesh, so mesh has texture pointer
struct Material
{
	std::shared_ptr<Texture2D> texture;
	TEXTURE_TYPE textureType = TEXTURE_TYPE::DIFFUSE;
	std::string path;
};


enum class MATERIAL_SET
{
	NONE,           // none map
	D,              // D: diffuseMap
	D_S,            // S: specular
	// D_S_N,          // N: normalMap
	D_S_N_H         // H: heightMap
};

class Mesh
{
public:
	explicit Mesh(const std::vector<MeshVertex> vertices, const std::vector<unsigned int> indices)
		:m_vertices(vertices), m_indices(indices)
	{
		initVAO();
	}
	

	void setMaterials(std::vector<Material> materials)
	{
		m_materials = materials;
	}

	void draw(const ShaderProgram& shader, std::vector<unsigned int> texUnits = {})
	{
		for(unsigned int i=0;i<m_materials.size();i++)
		{
			glActiveTexture(GL_TEXTURE0 + texUnits[i]);
			m_materials[0].texture->bindTexUnit(texUnits[i]);
		}	
		m_vao.bind();
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
		m_vao.unbind();
	}

	[[nodiscard]] bool isTexUnitMatching(const std::vector<unsigned int>& texUnits) const
	{
		return m_materials.size() == texUnits.size();
	}


private:
	void initVAO()
	{
		// vao bind vbo
		m_vbo.setData(m_vertices.data(), m_vertices.size());
		VertexAttrib positionAttrib = { 0, 3, GL_FLOAT, offsetof(MeshVertex, position) };
		VertexAttrib normalAttrib = { 1, 3, GL_FLOAT, offsetof(MeshVertex, normal) };
		VertexAttrib texCoordsAttrib = { 2, 2, GL_FLOAT, offsetof(MeshVertex, texCoords) };
		unsigned int bindingIndex = 0;
		m_vao.bindVertexBuffer(bindingIndex, m_vbo, 0);
		m_vao.bindVertexArrayAttrib(bindingIndex, positionAttrib);
		m_vao.bindVertexArrayAttrib(bindingIndex, normalAttrib);
		m_vao.bindVertexArrayAttrib(bindingIndex, texCoordsAttrib);
		// vao bind ebo
		m_ebo.setData(m_indices.data(), m_indices.size() * sizeof(unsigned int));
		m_vao.bindElementBuffer(m_ebo);
	}

private:
	VertexArray m_vao;
	VertexBuffer<MeshVertex> m_vbo;
	Buffer m_ebo;
	
	std::vector<MeshVertex> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<Material> m_materials;
};
