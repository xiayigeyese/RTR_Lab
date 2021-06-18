#pragma once

#include <glm/glm.hpp>

#include <array>
#include <app/geometry.h>

struct Cube_CS
{
	std::array<Cube::CubeVertex, 36> vertices;
};

double radicalInverse_VdC(glm::uint bits);

glm::vec2 hammersley(glm::uint i, glm::uint num);

void test_cpu_cull();

void test_gpu_cull();

void test_hiz();

void test_hiz_gpu();

void test_ssbo();

void test();