#pragma once
#include "DaemonEngine/Events/Event.h"

namespace Daemon
{

	class Layer
	{
	public:
		Layer(const std::string& debugName);
		virtual ~Layer() = default;

		virtual void OnAttach() { }
		virtual void OnDetach() { }
		virtual void OnUpdate(float deltaTime) { }
		virtual void OnGUI() { }
		virtual void OnEvent(Event& e) { }
	private:
		std::string m_DebugName;
	};

}
