#pragma once

#include <memory>

#include <glm/gtc/matrix_transform.hpp>

#include <opengl/opengl.h>
#include <app/geometry.h>

class IBLPass
{
public:
	explicit IBLPass(std::shared_ptr<Texture2D> envMap): m_cubeFaceTransforms({})
	{
		m_envMap = envMap;
		m_envCubeMapWidth  = 1024;
		m_envCubeMapHeight = 1024;
		m_irradianceMapWidth = 64;
		m_irradianceMapHeight = 64;
		m_prefilterMapWidth = 128;
		m_prefilterMapHeight = 128;
		m_brdfLutWidth = 512;
		m_brdfLutHeight = 512;
		m_envCubeMapMaxMipLevel = 8;
		m_prefilterMapMaxMipLevel = 5;
		
		init();
	}

	void setEnvCubeMapWH(const int envCubeMapWidth, const int envCubeMapHeight)
	{
		m_envCubeMapWidth = envCubeMapWidth;
		m_envCubeMapHeight = envCubeMapHeight;
	}

	/*[[nodiscard]] int get() const
	{
		
	}*/

	void setEnvCubeMapMaxMipLevel(const int envCubeMapMaxMipLevel)
	{
		m_envCubeMapMaxMipLevel = envCubeMapMaxMipLevel;
	}

	void setIrradianceMapWH(const int irradianceMapWidth, const int irradianceMapHeight)
	{
		m_irradianceMapWidth = irradianceMapWidth;
		m_irradianceMapHeight = irradianceMapHeight;
	}
	
	void setPrefilterMapWidth(const int prefilterMapWidth, const int prefilterMapHeight)
	{
		m_prefilterMapWidth = prefilterMapWidth;
		m_prefilterMapHeight = prefilterMapHeight;
	}
	
	void setPrefilterMapMaxMipLevel(const int prefilterMapMaxMipLevel)
	{
		m_prefilterMapMaxMipLevel = prefilterMapMaxMipLevel;
	}
	
	void setBRDfLutWidth(const int brdfLutWidth, const int brdfLutHeight)
	{
		m_brdfLutWidth = brdfLutWidth;
		m_brdfLutHeight = brdfLutHeight;
	}

	[[nodiscard]] int getPrefilterMapMaxMipLevel() const
	{
		return m_prefilterMapMaxMipLevel;
	}

	[[nodiscard]] std::shared_ptr<CubeMap> getEnvCubeMap() const 
	{
		return m_envCubeMap;
	}

	[[nodiscard]] std::shared_ptr<CubeMap> getIrradianceMap() const
	{
		return m_irradianceMap;
	}
	
	[[nodiscard]] std::shared_ptr<CubeMap> getPrefilterMap() const
	{
		return m_prefilterMap;
	}
	
	[[nodiscard]] std::shared_ptr<Texture2D> getBRDFLut() const
	{
		return m_brdfLut;
	}
	
	void renderPass()
	{
		render();
	}

private:
	void init();
	
	void initCubeFaceTransforms();

	void initShaders();

	void initFBOs();

	void render();

private:
	Framebuffer m_envCubeMapFBO, m_irradianceMapFBO, m_prefilterMapFBO, m_brdfLUTFBO;
	Renderbuffer m_envCubeMapFBODepth, m_irradianceMapFBODepth, m_prefilterMapFBODepth, m_brdfLUTFBODepth;
	ShaderProgram m_genEnvCubeMapShader, m_genIrrMapShader, m_genPrefilterMapShader, m_genBRDFlutShader;

	std::shared_ptr<Texture2D> m_envMap;
	std::shared_ptr<CubeMap> m_irradianceMap, m_envCubeMap, m_prefilterMap;
	std::shared_ptr<Texture2D> m_brdfLut;
	
	int m_envCubeMapWidth, m_envCubeMapHeight;
	int m_irradianceMapWidth, m_irradianceMapHeight;
	int m_prefilterMapWidth, m_prefilterMapHeight;
	int m_brdfLutWidth, m_brdfLutHeight;
	int m_envCubeMapMaxMipLevel, m_prefilterMapMaxMipLevel;

	std::array<glm::mat4, 6> m_cubeFaceTransforms;
	Cube m_cube;
	Quad m_quad;
};