#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

inline void setUniform(const GLuint uniformLocation, const bool& value)
{
	glUniform1i(uniformLocation, static_cast<GLuint>(value));
}

inline void setUniform(const GLuint uniformLocation, const int& value)
{
	glUniform1i(uniformLocation, value);
}

inline void setUniform(const GLuint uniformLocation, const float& value)
{
	glUniform1f(uniformLocation, value);
}

inline void setUniform(const GLuint uniformLocation, const glm::vec2& value)
{
	glUniform2fv(uniformLocation, 1, &value[0]);
}

inline void setUniform(const GLuint uniformLocation, const glm::vec3& value)
{
	glUniform3fv(uniformLocation, 1, &value[0]);
}

inline void setUniform(const GLuint uniformLocation, const glm::vec4& value)
{
	glUniform4fv(uniformLocation, 1, &value[0]);
}

inline void setUniform(const GLuint uniformLocation, const glm::mat2& mat)
{
	glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, &mat[0][0]);
}

inline void setUniform(const GLuint uniformLocation, const glm::mat3& mat)
{
	glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, &mat[0][0]);
}

inline void setUniform(const GLuint uniformLocation, const glm::mat4& mat)
{
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &mat[0][0]);
}


inline void setProgramUniform(const GLuint shaderProgram, const GLuint uniformLocation, const bool& value)
{
	glProgramUniform1i(shaderProgram, uniformLocation, static_cast<GLuint>(value));
}

inline void setProgramUniform(const GLuint shaderProgram, const GLuint uniformLocation, const int& value)
{
	glProgramUniform1i(shaderProgram, uniformLocation, value);
}

inline void setProgramUniform(const GLuint shaderProgram, const GLuint uniformLocation, const float& value)
{
	glProgramUniform1f(shaderProgram, uniformLocation, value);
}

inline void setProgramUniform(const GLuint shaderProgram, const GLuint uniformLocation, const glm::vec2& value)
{
	glProgramUniform2fv(shaderProgram, uniformLocation, 1, &value[0]);
}

inline void setProgramUniform(const GLuint shaderProgram, const GLuint uniformLocation, const glm::vec3& value)
{
	glProgramUniform3fv(shaderProgram, uniformLocation, 1, &value[0]);
}

inline void setProgramUniform(const GLuint shaderProgram, const GLuint uniformLocation, const glm::vec4& value)
{
	glProgramUniform4fv(shaderProgram, uniformLocation, 1, &value[0]);
}

inline void setProgramUniform(const GLuint shaderProgram, const GLuint uniformLocation, const glm::mat2& mat)
{
	glProgramUniformMatrix2fv(shaderProgram, uniformLocation, 1, GL_FALSE, &mat[0][0]);
}

inline void setProgramUniform(const GLuint shaderProgram, const GLuint uniformLocation, const glm::mat3& mat)
{
	glProgramUniformMatrix3fv(shaderProgram, uniformLocation, 1, GL_FALSE, &mat[0][0]);
}

inline void setProgramUniform(const GLuint shaderProgram, const GLuint uniformLocation, const glm::mat4& mat)
{
	glProgramUniformMatrix4fv(shaderProgram, uniformLocation, 1, GL_FALSE, &mat[0][0]);
}


template<typename T>
class UniformVariable
{
public:
	UniformVariable():m_location(-1){}

	explicit UniformVariable(const GLint location) :m_location(location) {}
	
	/*void setVariableValue(const T& value) const
	{
		assert(m_location!=-1);
		setUniform(m_location, value);
	}*/

	GLint getLocation() const
	{
		return m_location;
	}
	
private:
	GLint m_location;
};

