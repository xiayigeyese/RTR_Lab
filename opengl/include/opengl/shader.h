#pragma once

#include <iostream>
#include <cassert>
#include <string>
#include <fstream>
#include <sstream>

#include <glad/glad.h>

template<GLenum ShaderType>
class Shader
{
public:
	static constexpr GLenum shaderType = ShaderType;
	
	Shader():m_handler(0){}

	Shader(const Shader&) = delete;

	Shader(Shader&& shader) noexcept
		:m_handler(shader.getHandler())
	{
		shader.m_handler = 0;
	}

	Shader& operator=(const Shader&) = delete;

	Shader& operator=(Shader&& shader) noexcept
	{
		destroy();
		m_handler = shader.m_handler;
		shader.m_handler = 0;
		return *this;
	}

	~Shader()
	{
		destroy();
	}

	void destroy()
	{
		if (m_handler)
		{
			glDeleteShader(m_handler);
			m_handler = 0;
		}
	}

	void initFromSource(const std::string& source)
	{
		assert(!m_handler);
		m_handler = glCreateShader(ShaderType);
		const char* sourceChar = source.data();
		GLint len = static_cast<GLint>(source.length());
		glShaderSource(m_handler, 1, &sourceChar, &len);
		glCompileShader(m_handler);

		GLint success;
		GLchar infoLog[1024];
		glGetShaderiv(m_handler, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(m_handler, 1024, nullptr, infoLog);
			std::cout << "error shader compilation error " << infoLog << std::endl;
		}
	}

	void initFromFile(const std::string& shaderPath)
	{
		std::string source;
		std::ifstream sourceFS;
		sourceFS.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			sourceFS.open(shaderPath);
			std::stringstream sourceString;
			sourceString << sourceFS.rdbuf();
			sourceFS.close();
			source = sourceString.str();
		}
		catch (std::ifstream::failure& )
		{
			std::cout << "error shader file not successfully read" << std::endl;
		}
		initFromSource(source);
	}
	
	static Shader<ShaderType> loadFromSource(const std::string& source)
	{
		Shader<ShaderType> shader;
		shader.initFromSource(source);
		// return std::move(shader);
		return shader;
	}

	static Shader<ShaderType> loadFromFile(const std::string& shaderPath)
	{
		Shader<ShaderType> shader;
		shader.initFromFile(shaderPath);
		// return std::move(shader);
		return shader;
	}

	GLuint getHandler() const
	{
		return m_handler;                                         
	}

private:
	GLuint m_handler;
	
};
using VertexShader = Shader<GL_VERTEX_SHADER>;
using FragmentShader = Shader<GL_FRAGMENT_SHADER>;
using GeometryShader = Shader<GL_GEOMETRY_SHADER>;
using ComputerShader = Shader<GL_COMPUTE_SHADER>;