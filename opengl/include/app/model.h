#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

class Model
{
public:
	explicit Model(const std::string& path, MATERIAL_SET materialSet, unsigned assimpPostProcess = aiProcess_Triangulate)
	{
		loadModel(path, materialSet, assimpPostProcess);
	}

	void draw(const ShaderProgram& shader, std::vector<unsigned int> texUnits)
	{
		for(auto& mesh: m_meshes)
		{
			mesh.draw(shader, texUnits);
		}
	}

	bool isTexUnitMatching(const std::vector<unsigned int>& texUnits)
	{
		for (auto& mesh : m_meshes)
		{
			if (!mesh.isTexUnitMatching(texUnits)) return false;
		}
		return true;
	}

private:
	void loadModel(const std::string& path, MATERIAL_SET materialSet, unsigned assimpPostProcess = aiProcess_Triangulate);
	
	void processNode(aiNode* node);

	Mesh processMesh(aiMesh* mesh);

	void loadMaterials(MATERIAL_SET materialSet);
	
	Material loadMaterial(aiMaterial* material, aiTextureType type, TEXTURE_TYPE textureType);


private:
	std::vector<Mesh> m_meshes;
	std::string m_directory;
	std::vector<Material> m_materials;

	aiScene* m_aiScene;
	std::vector<aiMesh*> m_aiMeshes;
};