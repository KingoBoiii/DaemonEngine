#pragma once
#include "DaemonEngine/Core/Input/InputImpl.h"

namespace Daemon
{

	class Input
	{
	public:
		static Unique<Input> Create();
	public:
		Input(InputImpl* inputImpl);
		~Input();

		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPressed(MouseCode button);

		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	private:
		inline static InputImpl* s_InputImplementationInstance = nullptr;
	};

}
