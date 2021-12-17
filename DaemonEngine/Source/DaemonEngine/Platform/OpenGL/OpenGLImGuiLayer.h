#pragma once
#include "DaemonEngine/ImGui/ImGuiLayer.h"

namespace Daemon
{

	class OpenGLImGuiLayer : public ImGuiLayer
	{
	public:
		OpenGLImGuiLayer() : ImGuiLayer("GLImGui") { }
		virtual ~OpenGLImGuiLayer() = default;

		virtual void Begin() override;
		virtual void End() override;
		virtual void BlockEvents(bool block) override { m_BlockEvents = block; }

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;
	private:
		bool m_BlockEvents = true;
	};

}
