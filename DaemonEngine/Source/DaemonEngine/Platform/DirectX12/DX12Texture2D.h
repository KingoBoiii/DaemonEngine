#pragma once
#include "DaemonEngine/Renderer/Texture2D.h"
#include "DirectX12.h"
#include "d3dx12.h"

#pragma comment(lib, "dxguid.lib")

namespace Daemon
{

	class DX12Texture2D : public Texture2D
	{
	public:
		DX12Texture2D(const std::string& filepath, const TextureSpecification& specification);
		DX12Texture2D(uint32_t width, uint32_t height, const TextureSpecification& specification);
		DX12Texture2D(uint32_t width, uint32_t height, void* data, const TextureSpecification& specification);
		virtual ~DX12Texture2D();

		virtual void SetData(void* data) const override;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind() const override;

		virtual void* GetTexture() const override { return nullptr; }

		virtual bool operator==(const Texture2D& other) const override { return false; }

		ID3D12GraphicsCommandList* GetD3DX12CommandList() const { return m_CommandList; }
		ID3D12DescriptorHeap* GetD3DX12SRVHeap() const { return m_SRVHeap; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetD3DX12GPUDescriptorHandle() const { return m_SRVHeap->GetGPUDescriptorHandleForHeapStart(); }
	private:
		void CreateTexture();
		void CreateSRVHeap();
		void CreateCommandList();
	private:
		uint32_t m_Width;
		uint32_t m_Height;
		TextureSpecification m_Specification;
		void* m_LocalStorage = nullptr;

		ID3D12DescriptorHeap* m_SRVHeap;
		ID3D12Resource* m_Texture;

		// Testing
		ID3D12CommandAllocator* m_CommandAllocator;
		ID3D12GraphicsCommandList* m_CommandList;
	};

}
