#pragma once
#include "DaemonEngine/Core/Window.h"
#include "DaemonEngine/Renderer/RendererContext.h"

namespace Daemon
{

	class DX12SwapChain;

	class Win32Window : public Window
	{
	public:
		Win32Window(const WindowSpecification& specification);
		~Win32Window();

		virtual void Initialize() override;
		virtual void Update() override;

		virtual void Present() const override;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual bool GetVSync() const override { return m_Specification.VSync; }
		virtual void SetVSync(bool vsync) override {}
		virtual void SetTitle(const std::string& title) override;

		virtual void Maximize() override;

		// Getters
		virtual uint32_t GetWidth() const override { return m_Data.Width; }
		virtual uint32_t GetHeight() const override { return m_Data.Height; }
		virtual void* GetNativeWindowHandle() const override { return reinterpret_cast<HWND>(hWnd); }
		virtual const Shared<RendererContext>& GetRenderContext() const override { return m_RendererContext; }
		virtual const Shared<SwapChain>& GetSwapChain() const override { return m_SwapChain; }

		const HWND& GetHwnd() const { return hWnd; }
		const HDC& GetHDC() const { return hDc; }
	private:
		WindowSpecification m_Specification;
		Shared<RendererContext> m_RendererContext;
		Shared<SwapChain> m_SwapChain;

		HINSTANCE hInstance;
		HDC hDc;
		HWND hWnd;

		struct WindowData
		{
			EventCallbackFn EventCallback;
			uint32_t Width;
			uint32_t Height;
		};
		WindowData m_Data;

		friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};

}