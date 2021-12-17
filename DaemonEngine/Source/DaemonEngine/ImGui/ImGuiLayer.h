#pragma once
#include "DaemonEngine/Core/Layer.h"

namespace Daemon
{

	class ImGuiLayer : public Layer
	{
	public:
		static ImGuiLayer* Create();
	public:
		ImGuiLayer(const std::string& debugName) : Layer(debugName) { }
		virtual ~ImGuiLayer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void BlockEvents(bool block) = 0;
	};

}
