#include <string>
#include <iostream>

#include <stb_image.h>

#include <opengl/cube_map.h>


void CubeMap::loadFromFiles(const std::array<std::string, 6>& filePaths, const GLenum internalFormat, const GLenum format, const GLenum type) const
{
	assert(m_handler);

	// load data
	std::array<void*, 6> data = {};
	int width = 0, height = 0, nrChannels;
	for (int i = 0; i < 6; i++)
	{
		data[i] = stbi_load(filePaths[i].c_str(), &width, &height, &nrChannels, 0);
		if (data[i] == nullptr)
		{
			for (int j = 0; j < i; j++) stbi_image_free(data[j]);
			std::cout << "CubeMap: load cube map data failure" << std::endl;
			return;
		}
	}
	// set data and param
	setTexFormat(1, internalFormat, width, height);
	setTexImageData(width, height, format, type, data);
	setTexFilterParameter(GL_LINEAR, GL_LINEAR);
	setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	// free data
	for (int i = 0; i < 6; i++)
	{
		stbi_image_free(data[i]);
	}
}


