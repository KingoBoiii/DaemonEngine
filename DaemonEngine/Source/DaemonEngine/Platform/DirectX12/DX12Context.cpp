#include "kepch.h"
#include "DX12Context.h"

namespace Daemon
{

#define USE_WARP_DEVICE 0
#if USE_WARP_DEVICE 
    bool g_UseWarpDevice = true;
#else
    bool g_UseWarpDevice = false;
#endif
    D3D_FEATURE_LEVEL g_MinimumFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	DX12Context::DX12Context(HWND* window)
	{
        s_Instance = this;
		m_WindowHandle = window;
	}

	DX12Context::~DX12Context()
	{
	}

    void DX12Context::Init()
    {
#if defined(KE_DEBUG)
        HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugLayer)); // __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)
        KE_CORE_ASSERT(!FAILED(hr), "Failed to create Debug Interface");
        m_DebugLayer->EnableDebugLayer();
#endif
        CreateDevice(g_UseWarpDevice);
    }

	void DX12Context::CreateDevice(bool useWarpDevice)
	{
		CreateDXGIFactory(IID_PPV_ARGS(&m_Factory));

		if (useWarpDevice)
		{
			IDXGIAdapter* warpAdapter;
			HRESULT hr = m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
			KE_CORE_ASSERT(!FAILED(hr), "Failed to enumerate Warp Adapters");

			hr = D3D12CreateDevice(warpAdapter, g_MinimumFeatureLevel, IID_PPV_ARGS(&m_Device));
            KE_CORE_ASSERT(!FAILED(hr), "Failed to create a Warp Device");
        }
		else
		{
            IDXGIAdapter1* hardwareAdapter;
            GetHardwareAdapter(m_Factory, &hardwareAdapter);

            HRESULT hr = D3D12CreateDevice(hardwareAdapter, g_MinimumFeatureLevel, IID_PPV_ARGS(&m_Device));
            KE_CORE_ASSERT(!FAILED(hr), "Failed to create a Hardware Device");
        }
	}

	void DX12Context::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter)
	{
        *ppAdapter = nullptr;

        IDXGIAdapter1* adapter;
        IDXGIFactory6* factory6;
        if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            for (
                UINT adapterIndex = 0;
                DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
                adapterIndex, 
                requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS(&adapter));
                ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter, g_MinimumFeatureLevel, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }
        else
        {
            for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter, g_MinimumFeatureLevel, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }

        *ppAdapter = adapter;
	}

}