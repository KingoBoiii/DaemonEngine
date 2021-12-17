#pragma once
#include "DaemonEngine/ImGui/ImGuiLayer.h"

namespace Daemon
{

	class DX11ImGuiLayer : public ImGuiLayer
	{
	public:
		DX11ImGuiLayer() : ImGuiLayer("DX11ImGui") {}
		virtual ~DX11ImGuiLayer() = default;

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