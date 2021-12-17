#pragma once
#include <glm/glm.hpp>

namespace Daemon
{

	struct ShaderSpecification
	{
		std::string VSMainFunction;
		std::string PSMainFunction;
		std::string VSProfile;
		std::string PSProfile;

		ShaderSpecification()
		{
			VSMainFunction = "VSMain";
			PSMainFunction = "PSMain";
			VSProfile = "vs_5_0";
			PSProfile = "ps_5_0";
		}
	};

	class Shader
	{
	public:
		static Shared<Shader> Create(const std::string& filepath, const ShaderSpecification& specification = ShaderSpecification());
		static Shared<Shader> Create(const std::string& name, const std::string& filepath, const ShaderSpecification& specification = ShaderSpecification());
	public:
		virtual ~Shader() = default;

		virtual void SetInt(const std::string& name, int value) const = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) const = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& vector) const = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) const = 0;

		virtual const std::string& GetName() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
	};

	class ShaderLibrary
	{
	public:
		static Shared<ShaderLibrary> Create();
	public:
		ShaderLibrary() = default;
		~ShaderLibrary() = default;

		void Add(const Shared<Shader>& shader);
		void Load(const std::string& filepath);
		void Load(const std::string& name, const std::string& filepath);

		const Shared<Shader>& Get(const std::string& name) const;
	private:
		std::unordered_map<std::string, Shared<Shader>> m_Shaders;
	};

}
