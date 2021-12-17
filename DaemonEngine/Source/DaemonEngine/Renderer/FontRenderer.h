#pragma once
#include <glm/glm.hpp>

namespace Daemon
{

	class FontRenderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void DrawString(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color = glm::vec4(1.0f));
	private:
		static void SetupTextRendering();
	};

}
