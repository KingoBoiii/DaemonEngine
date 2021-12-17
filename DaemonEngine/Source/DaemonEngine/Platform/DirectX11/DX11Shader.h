#pragma once
#include "DaemonEngine/Renderer/Shader.h"

#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>

namespace Daemon
{

	struct DX11ShaderData
	{
		ID3DBlob* VertexSource;
		ID3DBlob* PixelSource; // Aka Fragment Source
		ID3D11VertexShader* VertexShader;
		ID3D11PixelShader* PixelShader; // Aka Fragment Shader
	};

	class DX11Shader : public Shader
	{
	public:
		DX11Shader(const std::string& filepath, const ShaderSpecification& specification);
		DX11Shader(const std::string& name, const std::string& filepath, const ShaderSpecification& specification);
		~DX11Shader();

		virtual void SetInt(const std::string& name, int value) const override {}
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) const override {}
		virtual void SetFloat3(const std::string& name, const glm::vec3& vector) const override {}
		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) const override {}

		virtual const std::string& GetName() const override { return m_Name; }

		virtual void Bind() const override;
		virtual void Unbind() const override;

		const DX11ShaderData& GetData() const { return m_ShaderData; }
	private:
		ID3DBlob* CompileFromFile(const std::wstring& filepath, const std::string& mainFunc, const std::string& profile);
		void GetNameByFilepath(const std::string& filepath);
	private:
		ShaderSpecification m_Specification;
		std::string m_Name;
		DX11ShaderData m_ShaderData;

		ID3D11Buffer* m_VSConstantBuffer = nullptr;
	};

}
