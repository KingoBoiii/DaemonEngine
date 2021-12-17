#pragma once
#include <string>
#include <functional>
#include <memory>

#include "DaemonEngine/Events/Event.h"
#include "DaemonEngine/Renderer/RendererContext.h"
#include "DaemonEngine/Renderer/SwapChain.h"

namespace Daemon
{

	struct WindowSpecification
	{
		std::string Title = "Kingo Engine";
		uint32_t Width = 1600;
		uint32_t Height = 900;
		bool Fullscreen = false;
		bool VSync = true;
	};

	class KINGO_API Window
	{
	public:
		static Unique<Window> Create(const WindowSpecification& specification = WindowSpecification());
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void Initialize() = 0;
		virtual void Update() = 0;

		// Presenting the image to the screen by the SwapChain. Aka Swap Buffer (Swapping Back Buffer and Presenting Buffer)
		virtual void Present() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual bool GetVSync() const = 0;
		virtual void SetVSync(bool vsync) = 0;
		virtual void SetTitle(const std::string& title) = 0;

		virtual void Maximize() = 0;

		// Getters
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void* GetNativeWindowHandle() const = 0;
		virtual const Shared<RendererContext>& GetRenderContext() const = 0;
		virtual const Shared<SwapChain>& GetSwapChain() const = 0;

		static void* GetNativeWindow() { return s_NativeWindowHandle; };
	protected:
		static void* s_NativeWindowHandle;
	};

}