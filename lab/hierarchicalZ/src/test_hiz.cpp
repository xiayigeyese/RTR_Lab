#include <iostream>
#include <vector>
#include <array>
#include <opengl/opengl.h>
#include <app/app.h>

#include "init.h"

using namespace std;
using namespace glm;

double radicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return static_cast<double>(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 hammersley(uint i, uint num)
{
	return vec2(static_cast<float>(i) / static_cast<float>(num), radicalInverse_VdC(i));
}


void test_hiz()
{
	// init app
	const int width = 1024, height = 1024;
	Application app("Hierarchi Z", width, height);
	GLFWwindow* window = app.getWindow();
	Input* input = app.getInput();

	// set camera
	Camera camera = Camera::perspectiveCamera(
		vec3(-2, 2, -4),
		vec3(0, 2, 0),
		vec3(0, 1, 0),
		45.0f,
		static_cast<float>(width) / height,
		0.1f,
		100.0f
	);
	CameraController cameraController(&camera, input);

	// set geometry
	Cube cube;
	Quad quad;

	// load texture
	Texture2D woodTex, containerTex;
	woodTex.loadFromFile("resources/textures/wood.png", Texture2D::MAX_MIPMAP);
	containerTex.loadFromFile("resources/textures/container2.png", Texture2D::MAX_MIPMAP);

	// depth texture
	Texture2D depthTex;
	int mipLevels = 1 + static_cast<int>(std::floorf(std::log2f(std::fmaxf(width, height))));
	depthTex.setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	depthTex.setTexFilterParameter(GL_NEAREST, GL_NEAREST);
	depthTex.setTexFormat(mipLevels, GL_DEPTH_COMPONENT32F, width, height);
	depthTex.genTexMipMap();

	// color texture
	Texture2D hiZImg;
	hiZImg.setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	hiZImg.setTexFilterParameter(GL_LINEAR, GL_LINEAR);
	hiZImg.setTexFormat(1, GL_RGBA16F, width, height);

	// load shader
	ShaderProgram hiZShader({
		VertexShader::loadFromFile("lab/hierarchicalZ/shaders/cube.vert").getHandler(),
		FragmentShader::loadFromFile("lab/hierarchicalZ/shaders/cube.frag").getHandler()
		});
	ShaderProgram hiZMipMapShader({
		VertexShader::loadFromFile("lab/hierarchicalZ/shaders/hier_Z_mipmap.vert").getHandler(),
		FragmentShader::loadFromFile("lab/hierarchicalZ/shaders/hier_Z_mipmap.frag").getHandler()
		});
	ShaderProgram showZBufferShader({
		VertexShader::loadFromFile("lab/hierarchicalZ/shaders/quad.vert").getHandler(),
		FragmentShader::loadFromFile("lab/hierarchicalZ/shaders/quad.frag").getHandler()
		});

	// FBO
	Framebuffer hiZFBO, hiZMipMapFBO;
	hiZFBO.attachTexture2D(GL_DEPTH_ATTACHMENT, depthTex, 0);
	hiZFBO.attachTexture2D(GL_COLOR_ATTACHMENT0, hiZImg);

	// shader set uniform
	UniformVariable<glm::mat4> hiZS_vs_mvp = hiZShader.getUniformVariable<glm::mat4>("u_mvp");
	hiZShader.setUniformValue("u_texMap", 0);
	hiZMipMapShader.setUniformValue("u_depthTex", 0);
	UniformVariable<int> hiZMipMapS_fs_mipLevel = hiZMipMapShader.getUniformVariable<int>("u_mipLevel");
	showZBufferShader.setUniformValue("u_depthTex", 0);

	// position
	int cubeCounts = 1000;
	vector<mat4> modelMatrices(cubeCounts);
	float rangeW = 100.f;
	for (int i = 0; i < cubeCounts; i++)
	{
		vec2 randomNumber = hammersley(i, cubeCounts);
		randomNumber = randomNumber * rangeW;
		glm::mat4 model = mat4(1.0f);
		model = glm::translate(model, glm::vec3(randomNumber.x, 0, randomNumber.y));
		float scale = (static_cast<float>(rand() % 20)) / 100.0f + 0.10f;
		model = glm::scale(model, glm::vec3(scale));
		modelMatrices[i] = model;
	}

	int columnCount = 10;
	vector<mat4> columnMatrices(columnCount);
	for (int i = 0; i < columnCount; i++)
	{
		vec2 randomNumber = hammersley(i, columnCount);
		randomNumber = randomNumber * rangeW / 1.5f;
		glm::mat4 model = mat4(1.0f);
		model = glm::translate(model, glm::vec3(randomNumber.x, 1, randomNumber.y));
		model = glm::scale(model, glm::vec3(1, 2, 1));
		columnMatrices[i] = model;
	}

	// render
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		app.getKeyPressInput();
		cameraController.processKeyPressInput();

		hiZFBO.bind();
		glViewport(0, 0, width, height);
		glClearColor(0.1f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		hiZShader.use();
		glActiveTexture(0);
		woodTex.bindTexUnit(0);
		mat4 vp = camera.getProjectionMatrix() * camera.getViewMatrix();
		for (int i = 0; i < cubeCounts; i++)
		{
			hiZShader.setUniformValue(hiZS_vs_mvp, vp * modelMatrices[i]);
			cube.draw();
		}
		glActiveTexture(0);
		containerTex.bindTexUnit(0);
		for (int i = 0; i < columnCount; i++)
		{
			hiZShader.setUniformValue(hiZS_vs_mvp, vp * columnMatrices[i]);
			cube.draw();
		}
		hiZShader.unUse();
		hiZFBO.unbind();

		hiZMipMapFBO.bind();
		hiZMipMapShader.use();
		glActiveTexture(0);
		depthTex.bindTexUnit(0);
		int currentWidth = width, currentHeight = height;
		for (int i = 1; i < mipLevels; i++)
		{
			currentWidth /= 2;
			currentHeight /= 2;
			currentWidth = currentWidth > 0 ? currentWidth : 1;
			currentHeight = currentHeight > 0 ? currentHeight : 1;
			hiZMipMapFBO.attachTexture2D(GL_DEPTH_ATTACHMENT, depthTex, i);
			depthTex.setTexParameteri(GL_TEXTURE_BASE_LEVEL, i - 1);
			depthTex.setTexParameteri(GL_TEXTURE_MAX_LEVEL, i - 1);
			glViewport(0, 0, currentWidth, currentHeight);
			glClear(GL_DEPTH_BUFFER_BIT);
			quad.draw();
		}
		hiZMipMapShader.unUse();
		hiZMipMapFBO.unbind();

		glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		showZBufferShader.use();
		glActiveTexture(0);
		depthTex.bindTexUnit(0);
		depthTex.setTexParameteri(GL_TEXTURE_BASE_LEVEL, 0);
		depthTex.setTexParameteri(GL_TEXTURE_MAX_LEVEL, 0);
		quad.draw();
		showZBufferShader.unUse();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}

void test_hiz_gpu()
{
	// init app
	const int width = 1024, height = 1024;
	Application app("gpu cull", width, height);
	GLFWwindow* window = app.getWindow();
	Input* input = app.getInput();

	// set camera
	Camera camera = Camera::perspectiveCamera(
		vec3(-2, 2, -4),
		vec3(0, 2, 0),
		vec3(0, 1, 0),
		45.0f,
		static_cast<float>(width) / height,
		0.1f,
		100.0f
	);
	CameraController cameraController(&camera, input);

	// set geometry
	Quad quad;

	// load texture
	Texture2D woodTex, containerTex;
	woodTex.loadFromFile("resources/textures/wood.png", Texture2D::MAX_MIPMAP);
	containerTex.loadFromFile("resources/textures/container2.png", Texture2D::MAX_MIPMAP);

	// depth texture
	Texture2D depthTex;
	int mipLevels = 1 + static_cast<int>(std::floorf(std::log2f(std::fmaxf(width, height))));
	depthTex.setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	depthTex.setTexFilterParameter(GL_NEAREST, GL_NEAREST);
	depthTex.setTexFormat(mipLevels, GL_DEPTH_COMPONENT32F, width, height);
	depthTex.genTexMipMap();

	// color texture
	Texture2D hiZImg;
	hiZImg.setTexWrapParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	hiZImg.setTexFilterParameter(GL_LINEAR, GL_LINEAR);
	hiZImg.setTexFormat(1, GL_RGBA16F, width, height);

	// load shader
	ShaderProgram hiZShader({
		VertexShader::loadFromFile("lab/hierarchicalZ/shaders/cube.vert").getHandler(),
		FragmentShader::loadFromFile("lab/hierarchicalZ/shaders/cube.frag").getHandler()
		});
	ShaderProgram hiZMipMapShader({
		VertexShader::loadFromFile("lab/hierarchicalZ/shaders/hier_Z_mipmap.vert").getHandler(),
		FragmentShader::loadFromFile("lab/hierarchicalZ/shaders/hier_Z_mipmap.frag").getHandler()
		});
	ShaderProgram csCullingShader({
		ComputerShader::loadFromFile("lab/hierarchicalZ/shaders/hier_Z_mipmap.comp").getHandler()
		});
	ShaderProgram showZBufferShader({
		VertexShader::loadFromFile("lab/hierarchicalZ/shaders/quad.vert").getHandler(),
		FragmentShader::loadFromFile("lab/hierarchicalZ/shaders/quad.frag").getHandler()
		});

	// FBO
	Framebuffer hiZFBO;
	hiZFBO.attachTexture2D(GL_DEPTH_ATTACHMENT, depthTex, 0);
	hiZFBO.attachTexture2D(GL_COLOR_ATTACHMENT0, hiZImg);

	Framebuffer hiZMipMapFBO;

	// shader set uniform
	UniformVariable<glm::mat4> hiZS_vs_mvp = hiZShader.getUniformVariable<glm::mat4>("u_mvp");
	hiZShader.setUniformValue("u_texMap", 0);
	hiZMipMapShader.setUniformValue("u_depthTex", 0);
	showZBufferShader.setUniformValue("u_depthTex", 0);
	UniformVariable<glm::mat4> csCullingS_cs_vp = csCullingShader.getUniformVariable<glm::mat4>("u_vpMatrix");
	csCullingShader.setUniformValue("u_zMipMap", 0);
	csCullingShader.setUniformValue("u_mipMaxLevel", mipLevels);
	csCullingShader.setUniformValue("u_screenWH", vec2(width, height));
	
	
	// position
	unsigned int cubeCount = 3190, columnCount = 10;
	unsigned int totalMeshCounts = cubeCount + columnCount;
	vector<Cube> cubes(totalMeshCounts);
	vector<mat4> modelMatrices(totalMeshCounts);
	float rangeW = 100.f;
	for (unsigned int i = 0; i < cubeCount; i++)
	{
		vec2 randomNumber = hammersley(i, cubeCount);
		randomNumber = randomNumber * rangeW;
		glm::mat4 model = mat4(1.0f);
		model = glm::translate(model, glm::vec3(randomNumber.x, 0, randomNumber.y));
		float scale = (static_cast<float>(rand() % 20)) / 100.0f + 0.10f;
		model = glm::scale(model, glm::vec3(scale));
		modelMatrices[i] = model;
	}
	for (unsigned int i = 0; i < columnCount; i++)
	{
		vec2 randomNumber = hammersley(i, columnCount);
		randomNumber = randomNumber * rangeW / 1.5f;
		glm::mat4 model = mat4(1.0f);
		model = glm::translate(model, glm::vec3(randomNumber.x, 1, randomNumber.y));
		model = glm::scale(model, glm::vec3(1, 2, 1));
		modelMatrices[cubeCount + i] = model;
	}

	// cs -> ssbo
	unsigned int csGroupCount = 100;
	Buffer cubeBuffer;
	cubeBuffer.setData(nullptr, totalMeshCounts * sizeof(std::array<Cube::CubeVertex, 36>), GL_DYNAMIC_COPY);
	auto* cubeCS = static_cast<Cube_CS*>(glMapNamedBufferRange(cubeBuffer.getHandle(), 0, totalMeshCounts * sizeof(std::array<Cube::CubeVertex, 36>), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	for (unsigned int i = 0; i < totalMeshCounts; i++)
	{
		cubeCS[i].vertices = cubes[i].getVertices();
	}
	glUnmapNamedBuffer(cubeBuffer.getHandle());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cubeBuffer.getHandle());

	Buffer worldMatrixBuffer;
	worldMatrixBuffer.setData(nullptr, totalMeshCounts * sizeof(glm::mat4), GL_DYNAMIC_COPY);
	auto* worldMatrixCS = static_cast<glm::mat4*>(glMapNamedBufferRange(worldMatrixBuffer.getHandle(), 0, totalMeshCounts * sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	for (unsigned int i = 0; i < totalMeshCounts; i++)
	{
		worldMatrixCS[i] = modelMatrices[i];
	}
	glUnmapNamedBuffer(worldMatrixBuffer.getHandle());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, worldMatrixBuffer.getHandle());

	Buffer cubeBufferNew;
	cubeBufferNew.setData(nullptr, totalMeshCounts * sizeof(std::array<Cube::CubeVertex, 36>), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, cubeBufferNew.getHandle());

	Buffer cubeIndexBuffer;
	vector<unsigned int> cubeIndices(totalMeshCounts);
	for (unsigned int i = 0; i < totalMeshCounts; i++)
	{
		cubeIndices[i] = i;
	}
	cubeIndexBuffer.setData(nullptr, totalMeshCounts * sizeof(unsigned int), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, cubeIndexBuffer.getHandle());

	Buffer atomicCounterBuffer;
	std::array<GLuint, 1> atomicCounter = { 0 };
	unsigned int newCubeCount = totalMeshCounts;
	atomicCounterBuffer.setData(atomicCounter.data(), sizeof(GLuint), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicCounterBuffer.getHandle());

	// render
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		app.getKeyPressInput();
		cameraController.processKeyPressInput();
		mat4 vp = camera.getProjectionMatrix() * camera.getViewMatrix();

		// draw cube
		hiZFBO.bind();
		glViewport(0, 0, width, height);
		glClearColor(0.1f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		hiZShader.use();
		glActiveTexture(0);
		for (unsigned int i = 0; i < newCubeCount; i++)
		{
			unsigned int cubeIndex = cubeIndices[i];
			if (cubeIndex < cubeCount)
			{
				woodTex.bindTexUnit(0);
			}
			else
			{
				containerTex.bindTexUnit(0);
			}
			hiZShader.setUniformValue(hiZS_vs_mvp, vp * modelMatrices[cubeIndex]);
			cubes[cubeIndex].draw();
		}
		hiZShader.unUse();
		hiZFBO.unbind();

		// hi-Z-mipmap
		hiZMipMapFBO.bind();
		hiZMipMapShader.use();
		glActiveTexture(0);
		depthTex.bindTexUnit(0);
		int currentWidth = width, currentHeight = height;
		for (int i = 1; i < mipLevels; i++)
		{
			currentWidth /= 2;
			currentHeight /= 2;
			currentWidth = currentWidth > 0 ? currentWidth : 1;
			currentHeight = currentHeight > 0 ? currentHeight : 1;
			hiZMipMapFBO.attachTexture2D(GL_DEPTH_ATTACHMENT, depthTex, i);
			depthTex.setTexParameteri(GL_TEXTURE_BASE_LEVEL, i - 1);
			depthTex.setTexParameteri(GL_TEXTURE_MAX_LEVEL, i - 1);
			glViewport(0, 0, currentWidth, currentHeight);
			glClear(GL_DEPTH_BUFFER_BIT);
			quad.draw();
		}
		hiZMipMapShader.unUse();
		hiZMipMapFBO.unbind();

		// show depth map
		glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		showZBufferShader.use();
		glActiveTexture(0);
		depthTex.bindTexUnit(0);
		depthTex.setTexParameteri(GL_TEXTURE_BASE_LEVEL, 0);
		depthTex.setTexParameteri(GL_TEXTURE_MAX_LEVEL, 0);
		quad.draw();
		showZBufferShader.unUse();

		// cull cube with gpu
		csCullingShader.use();
		csCullingShader.setUniformValue(csCullingS_cs_vp, vp);
		auto* atoCubeIndexCounter = static_cast<GLuint*>(glMapNamedBuffer(atomicCounterBuffer.getHandle(), GL_WRITE_ONLY));
		atoCubeIndexCounter[0] = 0;
		glUnmapNamedBuffer(atomicCounterBuffer.getHandle());
		glDispatchCompute(csGroupCount, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		csCullingShader.unUse();
		atoCubeIndexCounter = static_cast<GLuint*>(glMapNamedBuffer(atomicCounterBuffer.getHandle(), GL_READ_ONLY));
		newCubeCount = atoCubeIndexCounter[0];
		glUnmapNamedBuffer(atomicCounterBuffer.getHandle());
		auto* cubeIndeicesCS = static_cast<unsigned int*>(glMapNamedBufferRange(cubeIndexBuffer.getHandle(), 0, newCubeCount * sizeof(unsigned int), GL_MAP_READ_BIT));
		for (unsigned int i = 0; i < newCubeCount; i++)
		{
			cubeIndices[i] = cubeIndeicesCS[i];
		}
		glUnmapNamedBuffer(cubeIndexBuffer.getHandle());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}
