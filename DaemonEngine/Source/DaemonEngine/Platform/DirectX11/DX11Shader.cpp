#include "kepch.h"
#include "DX11Shader.h"
#include "DX11Context.h"

#include <glm/gtc/type_ptr.hpp>

#pragma comment(lib, "d3dcompiler")

namespace Daemon
{

	DX11Shader::DX11Shader(const std::string& filepath, const ShaderSpecification& specification)
		: m_Name("Unnamed"), m_Specification(specification)
	{
		GetNameByFilepath(filepath);
		std::wstring wfilepath = std::wstring(filepath.begin(), filepath.end());

		m_ShaderData.VertexSource = CompileFromFile(wfilepath, m_Specification.VSMainFunction, m_Specification.VSProfile);
		m_ShaderData.PixelSource = CompileFromFile(wfilepath, m_Specification.PSMainFunction, m_Specification.PSProfile);

		DX11Context& context = DX11Context::Get();
		HRESULT hr = context.GetDevice()->CreateVertexShader(m_ShaderData.VertexSource->GetBufferPointer(), m_ShaderData.VertexSource->GetBufferSize(), NULL, &m_ShaderData.VertexShader);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create Vertex Shader");

		hr = context.GetDevice()->CreatePixelShader(m_ShaderData.PixelSource->GetBufferPointer(), m_ShaderData.PixelSource->GetBufferSize(), NULL, &m_ShaderData.PixelShader);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create Pixel Shader");
	}

	DX11Shader::DX11Shader(const std::string& name, const std::string& filepath, const ShaderSpecification& specification)
		: m_Name(name), m_Specification(specification)
	{
		std::wstring wfilepath = std::wstring(filepath.begin(), filepath.end());

		m_ShaderData.VertexSource = CompileFromFile(wfilepath, m_Specification.VSMainFunction, m_Specification.VSProfile);
		m_ShaderData.PixelSource = CompileFromFile(wfilepath, m_Specification.PSMainFunction, m_Specification.PSProfile);

		DX11Context& context = DX11Context::Get();
		HRESULT hr = context.GetDevice()->CreateVertexShader(m_ShaderData.VertexSource->GetBufferPointer(), m_ShaderData.VertexSource->GetBufferSize(), NULL, &m_ShaderData.VertexShader);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create Vertex Shader");

		hr = context.GetDevice()->CreatePixelShader(m_ShaderData.PixelSource->GetBufferPointer(), m_ShaderData.PixelSource->GetBufferSize(), NULL, &m_ShaderData.PixelShader);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create Pixel Shader");
	}

	DX11Shader::~DX11Shader()
	{
		ReleaseCOM(m_ShaderData.VertexShader);
		ReleaseCOM(m_ShaderData.VertexSource);
		ReleaseCOM(m_ShaderData.PixelShader);
		ReleaseCOM(m_ShaderData.PixelSource);
		ReleaseCOM(m_VSConstantBuffer);
	}

	void DX11Shader::Bind() const
	{
		DX11Context& context = DX11Context::Get();
		context.GetDeviceContext()->VSSetShader(m_ShaderData.VertexShader, NULL, 0);
		context.GetDeviceContext()->PSSetShader(m_ShaderData.PixelShader, NULL, 0);
	}

	void DX11Shader::Unbind() const
	{
	}

	ID3DBlob* DX11Shader::CompileFromFile(const std::wstring& filepath, const std::string& mainFunc, const std::string& profile)
	{
		ID3DBlob* errorBlob = NULL;
		ID3DBlob* shaderBlob = NULL;
		HRESULT hr = D3DCompileFromFile(
			filepath.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			mainFunc.c_str(),
			profile.c_str(),
			D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG,
			0,
			&shaderBlob,
			&errorBlob);
		KE_CORE_ASSERT(!FAILED(hr) && !errorBlob && shaderBlob, ("Failed to compile shader\n{}", (char*)errorBlob->GetBufferPointer()));
		if (FAILED(hr))
		{
			if (errorBlob)
			{
				errorBlob->Release();
				return nullptr;
			}
			if (shaderBlob) { shaderBlob->Release(); }
		}
		return shaderBlob;
	}

	void DX11Shader::GetNameByFilepath(const std::string& filepath)
	{
		size_t found = filepath.find_last_of("/\\");
		m_Name = found != std::string::npos ? filepath.substr(found + 1) : filepath;
		found = m_Name.find_first_of(".");
		m_Name = found != std::string::npos ? m_Name.substr(0, found) : m_Name;
	}

}

