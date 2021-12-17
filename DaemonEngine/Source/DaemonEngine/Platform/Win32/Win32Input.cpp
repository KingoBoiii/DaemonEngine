#include "kepch.h"
#include "Win32Input.h"

#include <Windows.h>
#include <Windowsx.h>

namespace Daemon
{

	bool Win32Input::IsKeyPressed(KeyCode key)
	{
		if (key == Key::LeftControl)
		{
			key = 0xA2;
		}
		return GetKeyState(key) & 0x8000; // &(1 << 16);
	}

	bool Win32Input::IsMouseButtonPressed(MouseCode button)
	{
		return (GetKeyState(button + 1) & 0x8000) != 0;
	}

	glm::vec2 Win32Input::GetMousePosition()
	{
		POINT cursorPosition;
		GetCursorPos(&cursorPosition);
		return { static_cast<float>(cursorPosition.x), static_cast<float>(-cursorPosition.y) };
	}

}
