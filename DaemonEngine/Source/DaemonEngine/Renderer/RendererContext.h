#pragma once

struct GLFWwindow;

namespace Daemon
{

	class RendererContext
	{
	public:
		static Shared<RendererContext> Create(void* window);
	public:
		RendererContext() = default;
		virtual ~RendererContext() = default;

		virtual void Init() = 0;
	};

}
