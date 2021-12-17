#pragma once
#include "DaemonEngine/Renderer/Pipeline.h"
#include "DirectX12.h"

namespace Daemon
{

	class DX12SwapChain;
	class DX12VertexBuffer;

	class DX12Pipeline : public Pipeline
	{
	public:
		DX12Pipeline(const PipelineSpecification& specification);
		virtual ~DX12Pipeline();

		virtual void Bind() const override;

		ID3D12PipelineState* GetD3DX12PipelineState() const { return m_PipelineState; }
		ID3D12RootSignature* GetD3DX12RootSignature() const { return m_RootSignature; }

		virtual PipelineSpecification GetSpecification() const { return m_Specification; }
		virtual PipelineSpecification GetSpecification() { return m_Specification; }
	private:
		void CreatePipelineState();
	private:
		DX12SwapChain* m_SwapChain;

		static const UINT m_FrameCount = 2;
		UINT m_FrameIndex;

		PipelineSpecification m_Specification;
		ID3D12PipelineState* m_PipelineState;
		ID3D12RootSignature* m_RootSignature;

		ID3D12DescriptorHeap* m_RTVHeap;
		UINT m_RTVDescriptorSize;
		ID3D12Resource* m_RenderTargets[m_FrameCount];

		friend class DX12Context;
		friend class DX12Renderer;
	};

}
