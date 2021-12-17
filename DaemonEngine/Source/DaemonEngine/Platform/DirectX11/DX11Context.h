#pragma once
#include "DaemonEngine/Renderer/RendererContext.h"
#include "DX11SwapChain.h"

#include "DirectX11.h"

namespace Daemon
{

	class DX11Context : public RendererContext
	{
	public:
		DX11Context(HWND* window);
		virtual ~DX11Context();

		virtual void Init() override;

		inline ID3D11Device4* GetDevice() const { return m_Device; }
		inline ID3D11DeviceContext4* GetDeviceContext() const { return m_DeviceContext; }
		inline const HWND& GetHWND() const { return *m_WindowHandle; }
	private:
		void PrintVendorInfo();
		std::string GetFeatureLevelAsString();
		std::string GetVendor(uint32_t vendorId);
	public:
		static inline DX11Context& Get() { return *s_Instance; }
	private:
		HWND* m_WindowHandle;

		ID3D11Device4* m_Device;
		ID3D11DeviceContext4* m_DeviceContext;

		D3D_FEATURE_LEVEL m_FeatureLevel;
	private:
		static DX11Context* s_Instance;
	};

}
