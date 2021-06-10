#include <pass/skyboxPass.h>

void SkyBoxPass::initSkyBox(const std::shared_ptr<CubeMap>& cubeMapPtr, bool isHDR)
{
	std::array<SkyBoxVertex, 36> skyboxVertices =
	{
		SkyBoxVertex{{-1.0f,  1.0f, -1.0f}},
		SkyBoxVertex{{-1.0f, -1.0f, -1.0f}},
		SkyBoxVertex{{ 1.0f, -1.0f, -1.0f}},
		SkyBoxVertex{{ 1.0f, -1.0f, -1.0f}},
		SkyBoxVertex{{ 1.0f,  1.0f, -1.0f}},
		SkyBoxVertex{{-1.0f,  1.0f, -1.0f}},
		SkyBoxVertex{{-1.0f, -1.0f,  1.0f}},
		SkyBoxVertex{{-1.0f, -1.0f, -1.0f}},
		SkyBoxVertex{{-1.0f,  1.0f, -1.0f}},
		SkyBoxVertex{{-1.0f,  1.0f, -1.0f}},
		SkyBoxVertex{{-1.0f,  1.0f,  1.0f}},
		SkyBoxVertex{{-1.0f, -1.0f,  1.0f}},
		SkyBoxVertex{{ 1.0f, -1.0f, -1.0f}},
		SkyBoxVertex{{ 1.0f, -1.0f,  1.0f}},
		SkyBoxVertex{{ 1.0f,  1.0f,  1.0f}},
		SkyBoxVertex{{ 1.0f,  1.0f,  1.0f}},
		SkyBoxVertex{{ 1.0f,  1.0f, -1.0f}},
		SkyBoxVertex{{ 1.0f, -1.0f, -1.0f}},
		SkyBoxVertex{{-1.0f, -1.0f,  1.0f}},
		SkyBoxVertex{{-1.0f,  1.0f,  1.0f}},
		SkyBoxVertex{{ 1.0f,  1.0f,  1.0f}},
		SkyBoxVertex{{ 1.0f,  1.0f,  1.0f}},
		SkyBoxVertex{{ 1.0f, -1.0f,  1.0f}},
		SkyBoxVertex{{-1.0f, -1.0f,  1.0f}},
		SkyBoxVertex{{-1.0f,  1.0f, -1.0f}},
		SkyBoxVertex{{ 1.0f,  1.0f, -1.0f}},
		SkyBoxVertex{{ 1.0f,  1.0f,  1.0f}},
		SkyBoxVertex{{ 1.0f,  1.0f,  1.0f}},
		SkyBoxVertex{{-1.0f,  1.0f,  1.0f}},
		SkyBoxVertex{{-1.0f,  1.0f, -1.0f}},
		SkyBoxVertex{{-1.0f, -1.0f, -1.0f}},
		SkyBoxVertex{{-1.0f, -1.0f,  1.0f}},
		SkyBoxVertex{{ 1.0f, -1.0f, -1.0f}},
		SkyBoxVertex{{ 1.0f, -1.0f, -1.0f}},
		SkyBoxVertex{{-1.0f, -1.0f,  1.0f}},
		SkyBoxVertex{{ 1.0f, -1.0f,  1.0f}}
	};


	unsigned int bindingIndex = 0;
	VertexAttrib positionAttrib = { 0, 3, GL_FLOAT, offsetof(SkyBoxVertex, position) };
	m_vbo.setData(skyboxVertices.data(), skyboxVertices.size());
	m_skyBoxVao.bindVertexBuffer<SkyBoxVertex>(bindingIndex, m_vbo, 0);
	m_skyBoxVao.bindVertexArrayAttrib(bindingIndex, positionAttrib, GL_FALSE);

	m_cubeMapPtr = cubeMapPtr;
	if (isHDR)
	{
		m_shader.initShaders({
			VertexShader::loadFromFile("src/shaders/skybox/skybox.vert").getHandler(),
			FragmentShader::loadFromFile("src/shaders/skybox/skybox_hdr.frag").getHandler()
			});
	}
	else
	{
		m_shader.initShaders({
			VertexShader::loadFromFile("src/shaders/skybox/skybox.vert").getHandler(),
			FragmentShader::loadFromFile("src/shaders/skybox/skybox.frag").getHandler()
			});
	}

	m_shader.use();
	m_shader.setUniformValue<int>("skybox", 0);
	u_vs_view = m_shader.getUniformVariable<glm::mat4>("u_view");
	u_vs_projection = m_shader.getUniformVariable<glm::mat4>("u_projection");
}