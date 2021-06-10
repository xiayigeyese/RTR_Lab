#pragma once

#include <glad/glad.h>

struct VertexAttrib
{
	GLuint attribIndex;
	GLint size;
	GLenum type;
	GLuint relativeOffset;
};