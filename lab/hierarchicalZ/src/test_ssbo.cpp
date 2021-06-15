#include <iostream>
#include <vector>
#include <array>
#include <opengl/opengl.h>
#include <app/app.h>

#include "init.h"

using namespace std;
using namespace glm;

void test_ssbo()
{
	// init app
	const int width = 1024, height = 1024;
	Application app("ssbo", width, height);
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
}

void test()
{
	std::cout << 36 * sizeof(Cube::CubeVertex) << endl;
	std::cout << sizeof(std::array<Cube::CubeVertex, 36>) << endl;
}

