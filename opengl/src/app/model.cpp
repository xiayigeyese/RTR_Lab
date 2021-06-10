#include <app/model.h>

void Model::loadModel(const std::string& path, MATERIAL_SET materialSet, unsigned assimpPostProcess)
{
	// the data not in memory when beyond this function scope
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, assimpPostProcess);
	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
	{
		throw std::runtime_error(importer.GetErrorString());
	}
	m_aiScene = const_cast<aiScene*>(scene);
	m_directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode);
	loadMaterials(materialSet);
}

void Model::processNode(aiNode* node)
{
	// std::cout << node->mNumMeshes << std::endl;
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = m_aiScene->mMeshes[node->mMeshes[i]];
		m_aiMeshes.push_back(mesh);
		m_meshes.push_back(processMesh(mesh));
	}
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i]);
	}
}

Mesh Model::processMesh(aiMesh* mesh)
{
	std::vector<MeshVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Material> materials;
	// vertex
	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		glm::vec3 position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		glm::vec2 texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertices.push_back({ position, normal, texCoords });
	}

	// indices
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	return Mesh(vertices, indices);
}

void Model::loadMaterials(MATERIAL_SET materialSet)
{
	for (unsigned int i = 0; i < m_aiMeshes.size(); i++)
	{
		aiMaterial* material = m_aiScene->mMaterials[m_aiMeshes[i]->mMaterialIndex];
		if (materialSet == MATERIAL_SET::NONE)
		{
			m_meshes[i].setMaterials({});
		}
		else if (materialSet == MATERIAL_SET::D)
		{
			Material diffuseMap = loadMaterial(material, aiTextureType_DIFFUSE, TEXTURE_TYPE::DIFFUSE);
			m_meshes[i].setMaterials({ diffuseMap });
		}
		else if (materialSet == MATERIAL_SET::D_S)
		{
			Material diffuseMap = loadMaterial(material, aiTextureType_DIFFUSE, TEXTURE_TYPE::DIFFUSE);
			Material specularMap = loadMaterial(material, aiTextureType_SPECULAR, TEXTURE_TYPE::SPECULAR);
			m_meshes[i].setMaterials({ diffuseMap, specularMap });
		}
		else if (materialSet == MATERIAL_SET::D_S_N_H)
		{
			Material diffuseMap = loadMaterial(material, aiTextureType_DIFFUSE, TEXTURE_TYPE::DIFFUSE);
			Material specularMap = loadMaterial(material, aiTextureType_SPECULAR, TEXTURE_TYPE::SPECULAR);
			Material normalMap = loadMaterial(material, aiTextureType_NORMALS, TEXTURE_TYPE::NORMAL);
			Material heightMap = loadMaterial(material, aiTextureType_HEIGHT, TEXTURE_TYPE::NORMAL);
			m_meshes[i].setMaterials({ diffuseMap, specularMap, normalMap, heightMap });
		}
	}
}


Material Model::loadMaterial(aiMaterial* aiMaterial, aiTextureType type, TEXTURE_TYPE textureType)
{
	Material material;
	// std::cout << typeName << " " << material->GetTextureCount(type) << std::endl;
	unsigned int textureCount = aiMaterial->GetTextureCount(type);
	if (textureCount == 0)
	{
		throw std::runtime_error("error: the texture count is 0");
	}
	if (textureCount > 1)
	{
		std::cout << "warning: the texture count is " << textureCount << std::endl;
	}
	aiString str;
	aiMaterial->GetTexture(type, static_cast<unsigned int>(0), &str);
	bool skip = false;
	for (auto& mat : m_materials)
	{
		if (std::strcmp(mat.path.data(), str.C_Str()) == 0)
		{
			material = mat;
			skip = true;
			break;
		}
	}
	if (!skip)
	{
		Texture2D texture;
		texture.loadFromFile(m_directory + '/' + str.C_Str(), 4);
		Material mat;
		mat.path = str.C_Str();
		mat.texture = std::make_shared<Texture2D>(std::move(texture));
		mat.textureType = textureType;
		m_materials.push_back(mat);
		material = mat;
	}
	return material;
}
