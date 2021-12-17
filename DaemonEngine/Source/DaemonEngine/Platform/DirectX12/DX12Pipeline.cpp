#include "kepch.h"
#include "DX12Pipeline.h"
#include "DX12Context.h"

#include "DaemonEngine/Platform/DirectX12/DX12VertexBuffer.h"
#include "DaemonEngine/Platform/Win32/Win32Window.h"

namespace Daemon
{

    namespace Utils
    {

        D3D12_PRIMITIVE_TOPOLOGY_TYPE KingoTopologyTypeToDX12(PrimitiveTopology topology)
        {
            switch (topology)
            {
                case PrimitiveTopology::Points:     return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
                case PrimitiveTopology::Lines:      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
                case PrimitiveTopology::Triangles:  return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
                case PrimitiveTopology::None:
                default: break;
            }
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
        }

        D3D_PRIMITIVE_TOPOLOGY KingoTopologyToDX12(PrimitiveTopology topology)
        {
            switch (topology)
            {
                case PrimitiveTopology::Points:     return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
                case PrimitiveTopology::Lines:      return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
                case PrimitiveTopology::Triangles:  return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                case PrimitiveTopology::None:
                default: break;
            }
            return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
        }

        DXGI_FORMAT KingoShaderTypeToDX12(ShaderDataType type)
        {
            switch (type)
            {
                case ShaderDataType::Float:  return DXGI_FORMAT_R32_FLOAT;
                case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
                case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
                case ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4:
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool:
                case ShaderDataType::None:
                default: break;
            }
            return DXGI_FORMAT_UNKNOWN;
        }

    }

	DX12Pipeline::DX12Pipeline(const PipelineSpecification& specification)
		: m_Specification(specification)
	{
        CreatePipelineState();
	}

	DX12Pipeline::~DX12Pipeline()
	{
	}

    void DX12Pipeline::Bind() const
    {
    }

    void DX12Pipeline::CreatePipelineState()
    {
#if 0
        Win32Window* window = reinterpret_cast<Win32Window*>(Window::GetNativeWindow());
        m_SwapChain = reinterpret_cast<DX12SwapChain*>(window->GetSwapChain());
        int width = window->GetWidth();
        int height = window->GetHeight();

        std::vector<BufferElement> layout = m_Specification.InputLayout.GetElements();
        D3D12_INPUT_ELEMENT_DESC* inputElementDescs = new D3D12_INPUT_ELEMENT_DESC[layout.size()];
        for (uint32_t i = 0; i < layout.size(); i++)
        {
            const BufferElement& element = layout[i];
            DXGI_FORMAT format = Utils::KingoShaderTypeToDX12(element.Type);
            inputElementDescs[i] = { element.Name.c_str(), 0, format, 0, (uint32_t)element.Offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
        }

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
        rasterizerDesc.FillMode = m_Specification.Wireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
        rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
        rasterizerDesc.FrontCounterClockwise = TRUE;
        rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        rasterizerDesc.DepthClipEnable = TRUE;
        rasterizerDesc.MultisampleEnable = FALSE;
        rasterizerDesc.AntialiasedLineEnable = FALSE;
        rasterizerDesc.ForcedSampleCount = 0;
        rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        D3D12_BLEND_DESC blendStateDesc;
        ZeroMemory(&blendStateDesc, sizeof(D3D12_BLEND_DESC));
        blendStateDesc.AlphaToCoverageEnable = FALSE;
        blendStateDesc.IndependentBlendEnable = FALSE;
        const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {
            FALSE, FALSE,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_LOGIC_OP_NOOP,
            D3D12_COLOR_WRITE_ENABLE_ALL
        };
        for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        {
            blendStateDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
        }

        DX12Shader* shader = reinterpret_cast<DX12Shader*>(m_Specification.Shader.get());
        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout.NumElements = static_cast<uint32_t>(layout.size());
        psoDesc.InputLayout.pInputElementDescs = inputElementDescs;
        m_RootSignature = psoDesc.pRootSignature = shader->GetD3DX12RootSignature();
        psoDesc.VS = shader->GetD3DX12VertexShader();
        psoDesc.PS = shader->GetD3DX12PixelShader();
        psoDesc.RasterizerState = rasterizerDesc;
        psoDesc.BlendState = blendStateDesc;
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = Utils::KingoTopologyTypeToDX12(m_Specification.Topology);
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;

        ID3D12Device8* device = DX12Context::Get().GetDevice();
        HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
        KE_CORE_ASSERT(!FAILED(hr), "Failed to create Graphics Pipeline state");
#endif
    }

}