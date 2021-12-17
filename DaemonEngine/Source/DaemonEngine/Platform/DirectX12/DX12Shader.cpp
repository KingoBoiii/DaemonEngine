#include "kepch.h"
#include "DX12Shader.h"
#include "DX12Context.h"

namespace Daemon
{

	DX12Shader::DX12Shader(const std::string& filepath, const ShaderSpecification& specification)
		: m_Name("Unnamed"), m_Specification(specification)
	{
		CreateRootSignature();

		VertexShader = CompileFromFile(filepath, m_Specification.VSMainFunction, m_Specification.VSProfile);
		PixelShader = CompileFromFile(filepath, m_Specification.PSMainFunction, m_Specification.PSProfile);

		GetNameByFilepath(filepath);
	}

	DX12Shader::DX12Shader(const std::string& name, const std::string& filepath, const ShaderSpecification& specification)
		: m_Name(name), m_Specification(specification)
	{
		CreateRootSignature();

		VertexShader = CompileFromFile(filepath, m_Specification.VSMainFunction, m_Specification.VSProfile);
		PixelShader = CompileFromFile(filepath, m_Specification.PSMainFunction, m_Specification.PSProfile);
	}

	DX12Shader::~DX12Shader()
	{
	}

	void DX12Shader::CreateRootSignature()
	{
		D3D12_DESCRIPTOR_RANGE1 ranges[2];
		ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		ranges[0].NumDescriptors = 1;
		ranges[0].BaseShaderRegister = 0;
		ranges[0].RegisterSpace = 0;
		ranges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
		ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		ranges[1].NumDescriptors = 1;
		ranges[1].BaseShaderRegister = 0;
		ranges[1].RegisterSpace = 0;
		ranges[1].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
		ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_ROOT_PARAMETER1 rootParameters[2];
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
		rootParameters[0].DescriptorTable.pDescriptorRanges = &ranges[0];

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
		rootParameters[1].DescriptorTable.pDescriptorRanges = &ranges[1];

		D3D12_STATIC_SAMPLER_DESC sampler;
		ZeroMemory(&sampler, sizeof(D3D12_STATIC_SAMPLER_DESC));
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// Allow input layout and deny uneccessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;// |
			//D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		ZeroMemory(&rootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
		rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		rootSignatureDesc.Desc_1_1.NumParameters = _countof(rootParameters);
		rootSignatureDesc.Desc_1_1.pParameters = rootParameters;
		rootSignatureDesc.Desc_1_1.NumStaticSamplers = 1;
		rootSignatureDesc.Desc_1_1.pStaticSamplers = &sampler;
		//rootSignatureDesc.Desc_1_1.NumParameters = 0;
		//rootSignatureDesc.Desc_1_1.pParameters = nullptr;
		//rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
		//rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;
		rootSignatureDesc.Desc_1_1.Flags = rootSignatureFlags;

		ID3DBlob* signature;
		ID3DBlob* error;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
		KE_CORE_ASSERT(!FAILED(hr), ("Failed to Serialize root signature \n{0}", (char*)error->GetBufferPointer()));

		ID3D12Device8* device = DX12Context::Get().GetDevice();
		hr = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create root signature");
	}

	ID3DBlob* DX12Shader::CompileFromFile(const std::string& filepath, const std::string& mainFunc, const std::string& profile)
	{
#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif

		std::wstring wfilepath = std::wstring(filepath.begin(), filepath.end());
		ID3DBlob* errorMsg;
		ID3DBlob* shaderCode;
		HRESULT hr = D3DCompileFromFile(wfilepath.c_str(), nullptr, nullptr, mainFunc.c_str(), profile.c_str(), compileFlags, 0, &shaderCode, &errorMsg);
		KE_CORE_ASSERT(!FAILED(hr), ("Failed to compile shader: {0}\n{1}", mainFunc, (char*)errorMsg->GetBufferPointer()));
		if (FAILED(hr))
		{
			if (errorMsg)
			{
				errorMsg->Release();
				return nullptr;
			}
			if (shaderCode) { shaderCode->Release(); }
		}
		return shaderCode;
	}

	void DX12Shader::GetNameByFilepath(const std::string& filepath)
	{
		size_t found = filepath.find_last_of("/\\");
		m_Name = found != std::string::npos ? filepath.substr(found + 1) : filepath;
		found = m_Name.find_first_of(".");
		m_Name = found != std::string::npos ? m_Name.substr(0, found) : m_Name;
	}

}