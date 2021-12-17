#include "kepch.h"
#include "Application.h"
#include "Layer.h"

#include "DaemonEngine/Renderer/Renderer.h"
#include "DaemonEngine/Physics/2D/Physics2D.h"

extern bool g_ApplicationRunning;

namespace Daemon
{

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		s_Instance = this;

		WindowSpecification windowSpec;
		windowSpec.Title = specification.Name;
		windowSpec.Width = specification.WindowWidth;
		windowSpec.Height = specification.WindowHeight;
		windowSpec.Fullscreen = specification.Fullscreen;
		windowSpec.VSync = specification.VSync;
		m_Window = Window::Create(windowSpec);
		m_Window->Initialize();
		m_Window->SetEventCallback(KE_BIND_EVENT_FN(Application::OnEvent));
		m_Window->Maximize();
		m_Window->SetTitle(m_Specification.Name + " | " + GetConfigurationName() + " | " + GetPlatformName() + " | Renderer: " + RendererAPI::GetRendererName());

		m_Input = Input::Create();

		KE_CORE_TRACE("Application Information:");
		KE_CORE_TRACE("| Application Name: {}", m_Specification.Name);
		KE_CORE_TRACE("| Configuration: {}", GetConfigurationName());
		KE_CORE_TRACE("| Platform: {}", GetPlatformName());
		KE_CORE_TRACE("| Renderer: {}", RendererAPI::GetRendererName());

		Renderer::Initialize();
		Physics2D::Initialize();
			
		if (m_Specification.EnableImGui)
		{
			m_ImGuiLayer = ImGuiLayer::Create();
			PushOverlay(m_ImGuiLayer);
		}
	}
	Application::~Application()
	{
		Physics2D::Shutdown();
		Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Run()
	{
		OnInit();
		m_Timer = new Timer();

		while (m_IsRunning)
		{
			m_Window->Update();

			if (m_IsFocused && !m_IsMinimized)
			{
				float time = m_Timer->Elapsed();
				m_DeltaTime = time - m_LastFrameTime;
				m_LastFrameTime = time;

				Renderer::BeginFrame();
				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate(m_DeltaTime);
				}

				if (m_Specification.EnableImGui)
				{
					m_ImGuiLayer->Begin();
					for (Layer* layer : m_LayerStack)
					{
						layer->OnGUI();
					}
					m_ImGuiLayer->End();
				}
				Renderer::EndFrame();
				m_Window->Present();
			}
		}
		OnShutdown();
	}

	void Application::Restart()
	{
		m_IsRunning = false;
	}

	void Application::Close()
	{
		m_IsRunning = false;
		g_ApplicationRunning = false;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(KE_BIND_EVENT_FN(Application::OnWindowResizeEvent));
		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return OnWindowCloseEvent(e); });
		//dispatcher.Dispatch<WindowCloseEvent>(KE_BIND_EVENT_FN(Application::OnWindowCloseEvent));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			(*it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	bool Application::OnWindowResizeEvent(WindowResizeEvent& e)
	{
		uint32_t width = e.GetWidth();
		uint32_t height = e.GetHeight();
		m_IsMinimized = width <= 0 && height <= 0;

		Renderer::Resize(width, height);
		return false;
	}

	bool Application::OnWindowCloseEvent(WindowCloseEvent& e)
	{
		m_IsRunning = false;
		g_ApplicationRunning = false;
		return true;
	}

	bool Application::OnWindowFocusEvent(WindowFocusEvent& e)
	{
		m_IsFocused = e.IsFocused();
		return false;
	}

	const char* Application::GetConfigurationName()
	{
#if defined(KE_DEBUG)
		return "Debug";
#elif defined(KE_RELEASE)
		return "Release";
#elif defined(KE_DIST)
		return "Dist";
#else
#error Unknown configuration
#endif
	}

	const char* Application::GetPlatformName()
	{
#if defined(KE_PLATFORM_WINDOWS)
		return "Windows x64";
#else
#error Unknown platform?
#endif
	}


}
