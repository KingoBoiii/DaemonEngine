#pragma once
#include "DaemonEngine/Renderer/RendererContext.h"
#include "DirectX12.h"

#include "DX12VertexBuffer.h"
#include "DX12Shader.h"
#include "DX12Pipeline.h"
#include "DX12SwapChain.h"

namespace Daemon
{

	class DX12Context : public RendererContext
	{
	public:
		DX12Context(HWND* window);
		virtual ~DX12Context();

		virtual void Init() override;

		inline ID3D12Device8* GetDevice() const { return m_Device; }
		inline static DX12Context& Get() { return *s_Instance; }
	private:
		void CreateDevice(bool useWarpDevice);

		void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = false);
	private:
		HWND* m_WindowHandle;
		IDXGIFactory4* m_Factory;
		ID3D12Debug* m_DebugLayer;
		ID3D12Device8* m_Device;
	private:
		inline static DX12Context* s_Instance;
	};

}
