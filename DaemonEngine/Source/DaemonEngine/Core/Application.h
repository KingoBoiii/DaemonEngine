#pragma once
#include "Base.h"

#include "Window.h"
#include "LayerStack.h"
#include "Input/Input.h"

#include "DaemonEngine/Events/Event.h"
#include "DaemonEngine/Events/ApplicationEvent.h"
#include "DaemonEngine/Events/KeyEvent.h"
#include "DaemonEngine/ImGui/ImGuiLayer.h"
#include "Timer.h"

namespace Daemon
{

	struct ApplicationSpecification
	{
		std::string Name = "Daemon Engine";
		uint32_t WindowWidth = 1600;
		uint32_t WindowHeight = 900;
		bool Fullscreen = false;
		bool VSync = true;
		bool EnableImGui = true;
	};

	class KINGO_API Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void Run();
		void Restart();
		void Close();

		virtual void OnInit() {}
		virtual void OnShutdown() {}

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		// Configuration & platform
		static const char* GetConfigurationName();
		static const char* GetPlatformName();

		// Getters
		inline float GetFrameTime() const { return m_DeltaTime; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		inline Window& GetWindow() { return *m_Window; }
		static Application& Get() { return *s_Instance; }
		const ApplicationSpecification& GetSpecification() { return m_Specification; }
	private:
		bool OnWindowResizeEvent(WindowResizeEvent& e);
		bool OnWindowCloseEvent(WindowCloseEvent& e);
		bool OnWindowFocusEvent(WindowFocusEvent& e);
	private:
		ApplicationSpecification m_Specification;
		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		Timer* m_Timer;
		Unique<Window> m_Window;
		Unique<Input> m_Input;
		bool m_IsRunning = true;
		bool m_IsMinimized = false;
		bool m_IsFocused = true;
		float m_LastFrameTime = 0.0f;
		float m_DeltaTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();

}
