#include "kepch.h"
#include "DX11Context.h"

#include <Windows.h>
#include <DaemonEngine/Platform/Win32/Win32Window.h>

namespace Daemon
{

	DX11Context* DX11Context::s_Instance = nullptr;

	DX11Context::DX11Context(HWND* window)
		: m_WindowHandle(window)
	{
		s_Instance = this;
	}

	DX11Context::~DX11Context()
	{
		//ReleaseCOM(m_Device);
		//ReleaseCOM(m_DeviceContext);
	}

	void DX11Context::Init()
	{
		D3D_FEATURE_LEVEL levels[] = {
			D3D_FEATURE_LEVEL_9_1,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_11_1
		};

		UINT deviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED; //  D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(KE_DEBUG)
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
		HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, deviceFlags, NULL, 0, D3D11_SDK_VERSION, &device, &m_FeatureLevel, &deviceContext);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create DirectX 11 device");

		m_Device = reinterpret_cast<ID3D11Device4*>(device);
		m_DeviceContext = reinterpret_cast<ID3D11DeviceContext4*>(deviceContext);

		PrintVendorInfo();

		//ID3D11DeviceContext* defferedContext;
		//m_Device->CreateDeferredContext(0, &defferedContext);

		//ID3D11CommandList* cmdList;
		//defferedContext->FinishCommandList(true, &cmdList);
		//ID3D11Asynchronous* async;
		//defferedContext->Begin(async);
		//defferedContext->End(async);
		//defferedContext->Flush();
	}

	void DX11Context::PrintVendorInfo()
	{
		IDXGIFactory* factory;
		CreateDXGIFactory(IID_PPV_ARGS(&factory));

		IDXGIAdapter* adapter;
		KE_CORE_ASSERT(factory->EnumAdapters(0, &adapter) != DXGI_ERROR_NOT_FOUND, "Failed to find a suitable GPU for DirectX 11");

		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		KE_CORE_TRACE("----------------------------------------------");
		KE_CORE_TRACE("DirectX Info:");
		KE_CORE_TRACE("| Vendor: {0}", GetVendor(desc.VendorId));
		char* renderer = reinterpret_cast<char*>(malloc(sizeof(desc.Description)));
		wcstombs(renderer, desc.Description, sizeof(desc.Description));
		KE_CORE_TRACE("| Renderer: {0}", renderer);
		KE_CORE_TRACE("| Feature Lvl: {0}", GetFeatureLevelAsString());
		KE_CORE_TRACE("----------------------------------------------");
	}

	std::string DX11Context::GetFeatureLevelAsString()
	{
		switch (m_FeatureLevel)
		{
			case D3D_FEATURE_LEVEL_9_1: return "Direct 3D 9.1";
			case D3D_FEATURE_LEVEL_9_2: return "Direct 3D 9.2";
			case D3D_FEATURE_LEVEL_9_3: return "Direct 3D 9.3";
			case D3D_FEATURE_LEVEL_10_0: return "Direct 3D 10";
			case D3D_FEATURE_LEVEL_10_1: return "Direct 3D 10.1";
			case D3D_FEATURE_LEVEL_11_0: return "Direct 3D 11";
			case D3D_FEATURE_LEVEL_11_1: return "Direct 3D 11.1";
			default: return "Unknown feature level";
		}
	}

	std::string DX11Context::GetVendor(uint32_t vendorId)
	{
		switch (vendorId)
		{
			case 4318: return "NVIDIA Corporation";
			default: return std::to_string(vendorId);
		}
	}

}