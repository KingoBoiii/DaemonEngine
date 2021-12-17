#pragma once
#include "DaemonEngine/Renderer/Shader.h"
#include "DirectX12.h"

namespace Daemon
{

	class DX12Shader : public Shader
	{
	public:
		DX12Shader(const std::string& filepath, const ShaderSpecification& specification);
		DX12Shader(const std::string& name, const std::string& filepath, const ShaderSpecification& specification);
		virtual ~DX12Shader();

		virtual void SetInt(const std::string& name, int value) const override {}
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) const override {}
		virtual void SetFloat3(const std::string& name, const glm::vec3& vector) const override {}
		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) const override {}

		virtual const std::string& GetName() const override { return m_Name; }

		virtual void Bind() const override {}
		virtual void Unbind() const override {}

		D3D12_SHADER_BYTECODE GetD3DX12VertexShader() const { return { VertexShader->GetBufferPointer(), VertexShader->GetBufferSize() }; }
		D3D12_SHADER_BYTECODE GetD3DX12PixelShader() const { return { PixelShader->GetBufferPointer(), PixelShader->GetBufferSize() }; }
		ID3D12RootSignature* GetD3DX12RootSignature() const { return m_RootSignature; }
	private:
		void CreateRootSignature();
		ID3DBlob* CompileFromFile(const std::string& filepath, const std::string& mainFunc, const std::string& profile);
		void GetNameByFilepath(const std::string& filepath);
	private:
		std::string m_Name;
		ShaderSpecification m_Specification;

		ID3D12RootSignature* m_RootSignature;
		ID3DBlob* VertexShader;
		ID3DBlob* PixelShader; // Aka Fragment Shader
	};

}
