#pragma once
#include "DaemonEngine/Renderer/Shader.h"

namespace Daemon
{

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& filepath);
		~OpenGLShader();

		virtual void SetInt(const std::string& name, int value) const override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) const override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& vector) const override;
		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) const override;

		virtual const std::string& GetName() const override { return m_Name; }

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		std::string ReadFile(const std::string& filepath);
		void PreProcess(const std::string& source, std::string** shaders);
		uint32_t CreateShader(const char* source, uint32_t type);
		uint32_t CreateShaderProgram(uint32_t vertex, uint32_t fragment);

		void GetActiveUniforms();
		void GetNameByFilepath(const std::string& filepath);
	private:
		uint32_t m_RendererID;
		std::string m_Name;
		std::string m_VertexSource;
		std::string m_FragmentSource;
		std::unordered_map<std::string, int32_t> m_UniformLocations;
		
		enum class ShaderType
		{
			UNKNOWN = -1,
			VERTEX,
			FRAGMENT
		};
	};

}