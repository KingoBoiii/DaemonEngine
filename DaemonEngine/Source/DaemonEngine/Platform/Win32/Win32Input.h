#pragma once
#include "DaemonEngine/Core/Input/InputImpl.h"

namespace Daemon
{

	class Win32Input : public InputImpl
	{
	public:
		virtual bool IsKeyPressed(KeyCode keycode) override;
		virtual bool IsMouseButtonPressed(MouseCode button) override;

		virtual glm::vec2 GetMousePosition() override;
		virtual float GetMouseX() override { return GetMousePosition().x; }
		virtual float GetMouseY() override { return GetMousePosition().y; }
	};

}