#include <pass/IBLPass.h>

void IBLPass::initCubeFaceTransforms()
{
	float near = 0.1f, far = 25.0f;
	float aspect = static_cast<float>(m_envCubeMapWidth) / static_cast<float>(m_envCubeMapHeight);
	glm::mat4 lightSpaceProjection = glm::perspective(glm::radians(90.0f), aspect, near, far);
	glm::vec3 eyePos = glm::vec3(0, 0, 0);
	m_cubeFaceTransforms =
	{
		lightSpaceProjection * glm::lookAt(eyePos, eyePos + glm::vec3(1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)),
		lightSpaceProjection * glm::lookAt(eyePos, eyePos + glm::vec3(-1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)),
		lightSpaceProjection * glm::lookAt(eyePos, eyePos + glm::vec3(0.0,  1.0,  0.0), glm::vec3(0.0,  0.0,  1.0)),
		lightSpaceProjection * glm::lookAt(eyePos, eyePos + glm::vec3(0.0, -1.0,  0.0), glm::vec3(0.0,  0.0, -1.0)),
		lightSpaceProjection * glm::lookAt(eyePos, eyePos + glm::vec3(0.0,  0.0,  1.0), glm::vec3(0.0, -1.0,  0.0)),
		lightSpaceProjection * glm::lookAt(eyePos, eyePos + glm::vec3(0.0,  0.0, -1.0), glm::vec3(0.0, -1.0,  0.0)),
	};
}


void IBLPass::initShaders()
{
	m_genEnvCubeMapShader.initShaders({
		VertexShader::loadFromFile("src/shaders/IBL/envMap_to_envCubeMap.vert").getHandler(),
		FragmentShader::loadFromFile("src/shaders/IBL/envMap_to_envCubeMap.frag").getHandler()
		});
	m_genIrrMapShader.initShaders({
		VertexShader::loadFromFile("src/shaders/IBL/envCubeMap_to_irrMap.vert").getHandler(),
		FragmentShader::loadFromFile("src/shaders/IBL/envCubeMap_to_irrMap.frag").getHandler()
		});
	m_genPrefilterMapShader.initShaders({
		VertexShader::loadFromFile("src/shaders/IBL/envCubeMap_to_prefilterMap.vert").getHandler(),
		FragmentShader::loadFromFile("src/shaders/IBL/envCubeMap_to_prefilterMap.frag").getHandler()
	});
	m_genBRDFlutShader.initShaders({
		VertexShader::loadFromFile("src/shaders/IBL/brdfLUT.vert").getHandler(),
		FragmentShader::loadFromFile("src/shaders/IBL/brdfLUT.frag").getHandler()
		});
}


void IBLPass::initFBOs()
{
	// envCubeFBO
	m_envCubeMapFBODepth.allocateStorage(GL_DEPTH_COMPONENT32F, m_envCubeMapWidth, m_envCubeMapHeight);
	m_envCubeMapFBO.attachRenderBuffer(GL_DEPTH_ATTACHMENT, m_envCubeMapFBODepth);
	m_envCubeMap = std::make_shared<CubeMap>();
	m_envCubeMap->setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	m_envCubeMap->setTexFilterParameter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	m_envCubeMap->setTexFormat(m_envCubeMapMaxMipLevel, GL_RGB16F, m_envCubeMapWidth, m_envCubeMapHeight);

	// irradianceMapFBO
	m_irradianceMapFBODepth.allocateStorage(GL_DEPTH_COMPONENT32F, m_irradianceMapWidth, m_irradianceMapHeight);
	m_irradianceMapFBO.attachRenderBuffer(GL_DEPTH_ATTACHMENT, m_irradianceMapFBODepth);
	m_irradianceMap = std::make_shared<CubeMap>();
	m_irradianceMap->setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	m_irradianceMap->setTexFilterParameter(GL_LINEAR, GL_LINEAR);
	m_irradianceMap->setTexFormat(1, GL_RGB16F, m_irradianceMapWidth, m_irradianceMapHeight);

	// prefilterMapFBO
	m_prefilterMap = std::make_shared<CubeMap>();
	m_prefilterMap->setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	m_prefilterMap->setTexFilterParameter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	m_prefilterMap->setTexFormat(m_prefilterMapMaxMipLevel, GL_RGBA32F, m_prefilterMapWidth, m_prefilterMapHeight);
	m_prefilterMap->genTexMipMap();

	// BRDFLutFBO
	m_brdfLUTFBODepth.allocateStorage(GL_DEPTH_COMPONENT32F, m_brdfLutWidth, m_brdfLutHeight);
	m_brdfLUTFBO.attachRenderBuffer(GL_DEPTH_ATTACHMENT, m_brdfLUTFBODepth);
	m_brdfLut = std::make_shared<Texture2D>();
	m_brdfLut->setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	m_brdfLut->setTexFilterParameter(GL_LINEAR, GL_LINEAR);
	m_brdfLut->setTexFormat(1, GL_RG16F, m_brdfLutWidth, m_brdfLutHeight);
	m_brdfLUTFBO.attachTexture2D(GL_COLOR_ATTACHMENT0, *(m_brdfLut.get()));
}


void IBLPass::init()
{
	// init mvp matrix
	initCubeFaceTransforms();

	// init fbo
	initFBOs();
	
	// init shader
	initShaders();
}

void IBLPass::render()
{
	// pass1 : convert environmentMap to envCubeMap
	glViewport(0, 0, m_envCubeMapWidth, m_envCubeMapHeight);
	m_envCubeMapFBO.bind();
	m_genEnvCubeMapShader.use();
	glActiveTexture(0);
	m_envMap->bindTexUnit(0);
	UniformVariable<glm::mat4> genEnvCubeMapS_vs_mvp = m_genEnvCubeMapShader.getUniformVariable<glm::mat4>("u_mvp");
	m_genEnvCubeMapShader.setUniformValue("u_envMap", 0);
	for (int i = 0; i < 6; i++)
	{
		m_envCubeMapFBO.attachCubeMapFace(GL_COLOR_ATTACHMENT0, *(m_envCubeMap.get()), 0, i);
		if (!m_envCubeMapFBO.isComplete())
		{
			std::cout << " envCubeMapFBO is not complete! " << std::endl;
			return;
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_genEnvCubeMapShader.setUniformValue(genEnvCubeMapS_vs_mvp, m_cubeFaceTransforms[i]);
		m_cube.draw();
	}
	m_genEnvCubeMapShader.unUse();
	m_envCubeMapFBO.unbind();
	// gen envCubeMap mipmap
	m_envCubeMap->genTexMipMap();

	// pass2 : convert envCubeMap to irradianceMap
	glViewport(0, 0, m_irradianceMapWidth, m_irradianceMapHeight);
	m_irradianceMapFBO.bind();
	m_genIrrMapShader.use();
	glActiveTexture(0);
	m_envCubeMap->bindTexUnit(0);
	UniformVariable<glm::mat4> genIrrMapS_vs_mvp = m_genIrrMapShader.getUniformVariable<glm::mat4>("u_mvp");
	m_genIrrMapShader.setUniformValue("u_envCubeMap", 0);
	for (int face = 0; face < 6; face++)
	{
		m_irradianceMapFBO.attachCubeMapFace(GL_COLOR_ATTACHMENT0, *(m_irradianceMap.get()), 0, face);
		if (!m_irradianceMapFBO.isComplete())
		{
			std::cout << "irradianceMapFBO is not complete! " << std::endl;
			return;
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_genIrrMapShader.setUniformValue(genIrrMapS_vs_mvp, m_cubeFaceTransforms[face]);
		m_cube.draw();
	}
	m_genIrrMapShader.unUse();
	m_irradianceMapFBO.unbind();

	// pass3 : gen prefilterMap from envCubeMap
	m_prefilterMapFBO.bind();
	m_genPrefilterMapShader.use();
	glActiveTexture(0);
	m_envCubeMap->bindTexUnit(0);
	UniformVariable<glm::mat4> genPrefilterMapS_vs_u_mvp = m_genPrefilterMapShader.getUniformVariable<glm::mat4>("u_mvp");
	UniformVariable<float> genPrefilterMapS_fs_u_roughness = m_genPrefilterMapShader.getUniformVariable<float>("u_roughness");
	m_genPrefilterMapShader.setUniformValue("u_envCubeMapWidth", static_cast<float>(m_envCubeMapWidth));
	m_genPrefilterMapShader.setUniformValue("u_envCubeMap", 0);
	float mipRatio = 1.0f;
	for (int level = 0; level < m_prefilterMapMaxMipLevel; level++)
	{
		int mipWidth = static_cast<int>(static_cast<float>(m_prefilterMapWidth) * mipRatio);
		int mipHeight = static_cast<int>(static_cast<float>(m_prefilterMapHeight) * mipRatio);
		mipRatio *= 0.5f;

		glViewport(0, 0, mipWidth, mipHeight);
		m_prefilterMapFBODepth.allocateStorage(GL_DEPTH_COMPONENT32F, mipWidth, mipHeight);
		m_prefilterMapFBO.attachRenderBuffer(GL_DEPTH_ATTACHMENT, m_prefilterMapFBODepth);

		float roughnessT = static_cast<float>(level) / static_cast<float>(m_prefilterMapMaxMipLevel - 1);
		m_genPrefilterMapShader.setUniformValue(genPrefilterMapS_fs_u_roughness, roughnessT);
		for (int face = 0; face < 6; face++)
		{
			m_prefilterMapFBO.attachCubeMapFace(GL_COLOR_ATTACHMENT0, *(m_prefilterMap.get()), level, face);
			if (!m_prefilterMapFBO.isComplete())
			{
				std::cout << " prefilterMapFBO is not complete! " << std::endl;
				return;
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_genPrefilterMapShader.setUniformValue(genPrefilterMapS_vs_u_mvp, m_cubeFaceTransforms[face]);
			m_cube.draw();
		}
	}
	m_genPrefilterMapShader.unUse();
	m_prefilterMapFBO.unbind();

	// pass4 : gen brdfLUT
	glViewport(0, 0, m_brdfLutWidth, m_brdfLutHeight);
	m_brdfLUTFBO.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_genBRDFlutShader.use();
	m_quad.draw();
	m_genBRDFlutShader.unUse();
	m_brdfLUTFBO.unbind();
}