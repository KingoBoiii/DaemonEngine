#include "kepch.h"
#include "DX11Pipeline.h"
#include "DX11Context.h"
#include "DX11Shader.h"

#include "DirectX11.h"

namespace Daemon
{
	
	namespace Utils
	{

		DXGI_FORMAT KingoShaderTypeToDirectX(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::Float:		return DXGI_FORMAT_R32_FLOAT;
				case ShaderDataType::Float2:	return DXGI_FORMAT_R32G32_FLOAT;
				case ShaderDataType::Float3:	return DXGI_FORMAT_R32G32B32_FLOAT;
				case ShaderDataType::Float4:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case ShaderDataType::Int:		return DXGI_FORMAT_R32G32B32A32_SINT;
				case ShaderDataType::None:
				default: break;
			}
			return DXGI_FORMAT_UNKNOWN;
		}

	}

	DX11Pipeline::DX11Pipeline(const PipelineSpecification& specification)
		: m_Specification(specification)
	{
		KE_CORE_ASSERT(specification.Shader, "Pipeline needs a shader");
		Invalidate();
	}

	DX11Pipeline::~DX11Pipeline()
	{
		ReleaseCOM(m_InputLayout);
		ReleaseCOM(m_RasterizerState);
	}

	void DX11Pipeline::Bind() const
	{
		DX11Context context = DX11Context::Get();
		ID3D11Device* device = context.GetDevice();
		ID3D11DeviceContext* deviceContext = context.GetDeviceContext();

		m_Specification.Shader->Bind();

		deviceContext->IASetInputLayout(m_InputLayout);			// Input-Assembler Stage (Input Layout)
		deviceContext->RSSetState(m_RasterizerState);			// Rasterizer Stage
	}

	void DX11Pipeline::Invalidate()
	{
		DX11Context context = DX11Context::Get();
		ID3D11Device* device = context.GetDevice();

		std::vector<BufferElement> layout = m_Specification.InputLayout.GetElements();
		D3D11_INPUT_ELEMENT_DESC* desc = new D3D11_INPUT_ELEMENT_DESC[layout.size()];
		for (uint32_t i = 0; i < layout.size(); i++)
		{
			const BufferElement& element = layout[i];
			desc[i] = { element.Name.c_str(), 0, Utils::KingoShaderTypeToDirectX(element.Type), 0, (uint32_t)element.Offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		}

		DX11Shader* shader = reinterpret_cast<DX11Shader*>(m_Specification.Shader.get());
		const DX11ShaderData& shaderData = shader->GetData();
		device->CreateInputLayout(desc, layout.size(), shaderData.VertexSource->GetBufferPointer(), shaderData.VertexSource->GetBufferSize(), &m_InputLayout);

		CreateRasterizerState();
	}

	void DX11Pipeline::CreateRasterizerState()
	{
		DX11Context context = DX11Context::Get();
		ID3D11Device* device = context.GetDevice();
		ID3D11DeviceContext* deviceContext = context.GetDeviceContext();

		D3D11_RASTERIZER_DESC desc;
		desc.FillMode = m_Specification.Wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.FrontCounterClockwise = true;
		desc.DepthClipEnable = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.AntialiasedLineEnable = false;
		desc.MultisampleEnable = false;
		desc.ScissorEnable = false;
		desc.SlopeScaledDepthBias = 0.0f;

		HRESULT hr = device->CreateRasterizerState(&desc, &m_RasterizerState);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create Rasterizer State");
	}

}