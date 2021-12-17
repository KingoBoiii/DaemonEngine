#include "kepch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Daemon
{

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: m_Name("Unnamed")
	{
		std::string& shaderSource = ReadFile(filepath);
		std::string* shaders[2] = { &m_VertexSource, &m_FragmentSource };
		PreProcess(shaderSource, shaders);

		uint32_t vertex = CreateShader(m_VertexSource.c_str(), GL_VERTEX_SHADER);
		uint32_t fragment = CreateShader(m_FragmentSource.c_str(), GL_FRAGMENT_SHADER);

		m_RendererID = CreateShaderProgram(vertex, fragment);

		GetNameByFilepath(filepath);
		GetActiveUniforms();
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& filepath)
		: m_Name(name)
	{
		std::string& shaderSource = ReadFile(filepath);
		std::string* shaders[2] = { &m_VertexSource, &m_FragmentSource };
		PreProcess(shaderSource, shaders);

		uint32_t vertex = CreateShader(m_VertexSource.c_str(), GL_VERTEX_SHADER);
		uint32_t fragment = CreateShader(m_FragmentSource.c_str(), GL_FRAGMENT_SHADER);

		m_RendererID = CreateShaderProgram(vertex, fragment);

		GetActiveUniforms();
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::SetInt(const std::string& name, int value) const
	{
		auto it = m_UniformLocations.find(name);
		if (it != m_UniformLocations.end())
		{
			int32_t location = it->second;
			glUniform1i(location, value);
		}
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count) const
	{
		auto it = m_UniformLocations.find(name);
		if (it != m_UniformLocations.end())
		{
			int32_t location = it->second;
			glUniform1iv(location, count, values);
		}
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& vector) const
	{
		auto it = m_UniformLocations.find(name);
		if (it != m_UniformLocations.end())
		{
			int32_t location = it->second;
			glUniform3f(location, vector.x, vector.y, vector.z);
		}
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& matrix) const
	{
		auto it = m_UniformLocations.find(name);
		if (it != m_UniformLocations.end())
		{
			int32_t location = it->second;
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
		}
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		std::ifstream file(filepath);
		if (file.is_open())
		{
			std::string line;
			std::string result;
			while (std::getline(file, line))
			{
				result += line + "\n";
			}
			file.close();
			return result;
		}
		KE_CORE_WARN("Failed to read shader, {0}", filepath);
		return std::string();
	}

	static std::vector<std::string> GetLines(const std::string& source)
	{
		size_t start = 0;
		size_t end = source.find_first_of("\n");

		std::vector<std::string> result;

		while (end <= std::string::npos)
		{
			std::string token = source.substr(start, end - start);
			if (!token.empty())
				result.push_back(token);

			if (end == std::string::npos)
				break;

			start = end + 1;
			end = source.find_first_of("\n", start);
		}

		return result;
	}

	void OpenGLShader::PreProcess(const std::string& source, std::string** shaders)
	{
		ShaderType type = ShaderType::UNKNOWN;

		std::vector<std::string> lines = GetLines(source);
		for (uint32_t i = 0; i < lines.size(); i++)
		{
			const char* str = lines[i].c_str();
			if (strstr(str, "#type"))
			{
				if (strstr(str, "vertex"))
				{
					type = ShaderType::VERTEX;
				}
				else if (strstr(str, "fragment") || strstr(str, "pixel"))
				{
					type = ShaderType::FRAGMENT;
				}
			}
			else if (type != ShaderType::UNKNOWN)
			{
				shaders[(int32_t)type]->append(str);
				shaders[(int32_t)type]->append("\n");
			}
		}
	}

	uint32_t OpenGLShader::CreateShader(const char* source, uint32_t type)
	{
		uint32_t shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);

		int32_t compileStatus;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus == GL_FALSE)
		{
			GLsizei log_length = 0;
			GLchar message[1024];
			glGetShaderInfoLog(shader, 1024, &log_length, message);
			// Write the error to a log
			KE_CORE_ERROR("[ERROR]: SHADER_COMPILATION\n{0}", message);
		}

		return shader;
	}

	uint32_t OpenGLShader::CreateShaderProgram(uint32_t vertex, uint32_t fragment)
	{
		uint32_t program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);

		GLint linkStatus;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus == GL_FALSE)
		{
			GLsizei log_length = 0;
			GLchar message[1024];
			glGetProgramInfoLog(program, 1024, &log_length, message);
			// Write the error to a log
			KE_CORE_ERROR("[ERROR]: PROGRAM_LINKING\n{0}", message);
		}

		return program;
	}

	void OpenGLShader::GetActiveUniforms()
	{
		int32_t activeUniformCount;
		glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, &activeUniformCount);
		m_UniformLocations.reserve(activeUniformCount);

		char name[128];
		int32_t length;
		int32_t size;
		uint32_t type;
		for (int32_t i = 0; i < activeUniformCount; i++)
		{
			glGetActiveUniform(m_RendererID, i, sizeof(name), &length, &size, &type, name);
			int32_t location = glGetUniformLocation(m_RendererID, name);
			m_UniformLocations[name] = location; 
		}
		KE_CORE_INFO("[INFO]: Shader loaded with {0} active uniforms", activeUniformCount);
	}

	void OpenGLShader::GetNameByFilepath(const std::string& filepath)
	{
		size_t found = filepath.find_last_of("/\\");
		m_Name = found != std::string::npos ? filepath.substr(found + 1) : filepath;
		found = m_Name.find_first_of(".");
		m_Name = found != std::string::npos ? m_Name.substr(0, found) : m_Name;
	}

}