#pragma once
#include "DaemonEngine/Core/KeyCodes.h"
#include "DaemonEngine/Core/MouseCodes.h"

#include <glm/glm.hpp>

namespace Daemon
{

	class InputImpl
	{
	public:
		virtual ~InputImpl() = default;

		virtual bool IsKeyPressed(KeyCode keycode) = 0;
		virtual bool IsMouseButtonPressed(MouseCode button) = 0;

		virtual glm::vec2 GetMousePosition() = 0;
		virtual float GetMouseX() = 0;
		virtual float GetMouseY() = 0;
	};

}