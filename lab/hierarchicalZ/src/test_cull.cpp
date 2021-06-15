#include <iostream>
#include <vector>
#include <array>
#include <opengl/opengl.h>
#include <app/app.h>

#include "init.h"

using namespace std;
using namespace glm;

void test_cpu_cull()
{
	// init app
	const int width = 1024, height = 1024;
	Application app("cpu cull", width, height);
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
	ShaderProgram showZBufferShader({
		VertexShader::loadFromFile("lab/hierarchicalZ/shaders/quad.vert").getHandler(),
		FragmentShader::loadFromFile("lab/hierarchicalZ/shaders/quad.frag").getHandler()
		});

	// FBO
	Framebuffer hiZFBO;
	hiZFBO.attachTexture2D(GL_DEPTH_ATTACHMENT, depthTex, 0);
	hiZFBO.attachTexture2D(GL_COLOR_ATTACHMENT0, hiZImg);

	// shader set uniform
	UniformVariable<glm::mat4> hiZS_vs_mvp = hiZShader.getUniformVariable<glm::mat4>("u_mvp");
	hiZShader.setUniformValue("u_texMap", 0);
	showZBufferShader.setUniformValue("u_depthTex", 0);

	// position
	int cubeCounts = 3200;
	int newCubeIndex = 0;
	vector<mat4> modelMatrices(cubeCounts);
	vector<Cube> cubes(cubeCounts), newCubes(cubeCounts);
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

		// cull cube with depth
		mat4 vp1 = camera.getProjectionMatrix() * camera.getViewMatrix();
		newCubeIndex = 0;
		for (int i = 0; i < cubeCounts; i++)
		{
			float minZ = 1.0f;
			for (int j = 0; j < 36; j++)
			{
				Cube::CubeVertex vertex = cubes[i].getVertex(j);
				vec4 position = vp1 * modelMatrices[i] * vec4(vertex.position, 1.0f);
				if (position.w < 0) continue;
				position = position / position.w;
				float depth = (position.z + 1) * 0.5f;
				minZ = std::min(minZ, depth);
			}
			if (minZ < 0.995f)
			{
				// copy vertex attributes
				std::array<Cube::CubeVertex, 36> cubeVertices{};
				for (int j = 0; j < 36; j++)
				{
					Cube::CubeVertex vertex = cubes[i].getVertex(j);
					cubeVertices[j].position = vec3(modelMatrices[i] * vec4(vertex.position, 1.0f));
					cubeVertices[j].normal = glm::transpose(glm::inverse(mat3(modelMatrices[j]))) * vertex.normal;
					cubeVertices[j].texCoords = vertex.texCoords;
				}
				newCubes[newCubeIndex].resetCubeVertex(cubeVertices);
				newCubeIndex++;
			}
		}

		hiZFBO.bind();
		glViewport(0, 0, width, height);
		glClearColor(0.1f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		hiZShader.use();
		glActiveTexture(0);
		woodTex.bindTexUnit(0);
		mat4 vp = camera.getProjectionMatrix() * camera.getViewMatrix();
		/*for (int i = 0; i < cubeCounts; i++)
		{
			hiZShader.setUniformValue(hiZS_vs_mvp, vp * modelMatrices[i]);
			cubes[i].draw();
		}*/
		for (int i = 0; i < newCubeIndex; i++)
		{
			hiZShader.setUniformValue(hiZS_vs_mvp, vp);
			newCubes[i].draw();
		}
		glActiveTexture(0);
		containerTex.bindTexUnit(0);
		for (int i = 0; i < columnCount; i++)
		{
			hiZShader.setUniformValue(hiZS_vs_mvp, vp * columnMatrices[i]);
			cubes[i].draw();
		}
		hiZShader.unUse();
		hiZFBO.unbind();

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

void test_gpu_cull()
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

	// shader set uniform
	UniformVariable<glm::mat4> hiZS_vs_mvp = hiZShader.getUniformVariable<glm::mat4>("u_mvp");
	hiZShader.setUniformValue("u_texMap", 0);
	showZBufferShader.setUniformValue("u_depthTex", 0);
	UniformVariable<glm::mat4> csCullingS_cs_vp = csCullingShader.getUniformVariable<glm::mat4>("u_vpMatrix");

	// position
	int cubeCounts = 3200;
	int newCubeIndex = 0;
	vector<mat4> modelMatrices(cubeCounts);
	vector<Cube> cubes(cubeCounts), newCubes(cubeCounts);
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

	// cs -> ssbo
	int csGroupCount = 100;
	Buffer cubeBuffer;
	cubeBuffer.setData(nullptr, cubeCounts * sizeof(std::array<Cube::CubeVertex, 36>), GL_DYNAMIC_COPY);
	auto* cubeCS = static_cast<Cube_CS*>(glMapNamedBufferRange(cubeBuffer.getHandle(), 0, cubeCounts * sizeof(std::array<Cube::CubeVertex, 36>), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	for(int i=0;i<cubeCounts;i++)
	{
		std::array<Cube::CubeVertex, 36> cubeVertices = cubes[i].getVertices();
		for(int j =0;j<36;j++)
		{
			cubeCS[i].vertices[j].position = cubeVertices[j].position;
			cubeCS[i].vertices[j].normal = cubeVertices[j].normal;
			cubeCS[i].vertices[j].texCoords = cubeVertices[j].texCoords;
		}
	}
	glUnmapNamedBuffer(cubeBuffer.getHandle());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cubeBuffer.getHandle());

	Buffer worldMatrixBuffer;
	worldMatrixBuffer.setData(nullptr, cubeCounts * sizeof(glm::mat4), GL_DYNAMIC_COPY);
	auto* worldMatrixCS = static_cast<glm::mat4*>(glMapNamedBufferRange(worldMatrixBuffer.getHandle(), 0, cubeCounts * sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	for(int i=0; i<cubeCounts;i++)
	{
		worldMatrixCS[i] = modelMatrices[i];
	}
	glUnmapNamedBuffer(worldMatrixBuffer.getHandle());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, worldMatrixBuffer.getHandle());

	Buffer cubeBufferNew;
	cubeBufferNew.setData(nullptr, cubeCounts * sizeof(Cube::CubeVertex), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, cubeBufferNew.getHandle());

	Buffer atomicCounterBuffer;
	std::array<GLuint, 1> atomicCounter = { 0 };
	atomicCounterBuffer.setData(atomicCounter.data(), sizeof(GLuint), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicCounterBuffer.getHandle());
	
	// render
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		app.getKeyPressInput();
		cameraController.processKeyPressInput();

		// cull cube with gpu
		mat4 vp1 = camera.getProjectionMatrix() * camera.getViewMatrix();
		csCullingShader.use();
		csCullingShader.setUniformValue(csCullingS_cs_vp, vp1);
		atomicCounterBuffer.setData(atomicCounter.data(), sizeof(GLuint), GL_DYNAMIC_COPY);
		glDispatchCompute(csGroupCount, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		auto* atoCubeIndexCounter = static_cast<GLuint*>(glMapNamedBuffer(atomicCounterBuffer.getHandle(), GL_READ_ONLY));
		std::cout << atoCubeIndexCounter[0] << endl;
		csCullingShader.unUse();

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
			cubes[i].draw();
		}
		/*for (int i = 0; i < newCubeIndex; i++)
		{
			hiZShader.setUniformValue(hiZS_vs_mvp, vp);
			newCubes[i].draw();
		}*/
		glActiveTexture(0);
		containerTex.bindTexUnit(0);
		for (int i = 0; i < columnCount; i++)
		{
			hiZShader.setUniformValue(hiZS_vs_mvp, vp * columnMatrices[i]);
			cubes[i].draw();
		}
		hiZShader.unUse();
		hiZFBO.unbind();

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