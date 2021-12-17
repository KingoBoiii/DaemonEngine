#pragma once
#include "DaemonEngine/Core/Window.h"
#include "DaemonEngine/Renderer/RendererContext.h"

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

struct GLFWwindow;

namespace Daemon
{

	class KINGO_API GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowSpecification& specification);
		~GLFWWindow();

		virtual void Initialize() override;
		virtual void Update() override;

		virtual void Present() const override;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual bool GetVSync() const override;
		virtual void SetVSync(bool vsync) override;
		virtual void SetTitle(const std::string& title) override;

		virtual void Maximize() override;

		// Getters
		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }
		virtual void* GetNativeWindowHandle() const override { return glfwGetWin32Window(m_Window); }
		virtual const Shared<RendererContext>& GetRenderContext() const override { return m_RendererContext; }
		virtual const Shared<SwapChain>& GetSwapChain() const override { return m_SwapChain; }
	private:
		void SetupCallbacks();
	private:
		GLFWwindow* m_Window;
		Shared<RendererContext> m_RendererContext;
		Shared<SwapChain> m_SwapChain;

		WindowSpecification m_Specification;
		struct WindowData
		{
			bool VSync;

			EventCallbackFn EventCallback;
		};
		WindowData m_Data;

		friend class GLFWInput;
	};

}
