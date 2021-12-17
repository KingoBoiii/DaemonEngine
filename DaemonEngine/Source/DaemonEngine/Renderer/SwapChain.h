#pragma once

namespace Daemon
{

	struct SwapChainSpecification
	{
		uint32_t Width;
		uint32_t Height;
		void* WindowHandle;
	};

	class SwapChain
	{
	public:
		static Shared<SwapChain> Create(const SwapChainSpecification& specification = SwapChainSpecification());
	public:
		virtual ~SwapChain() = default;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void Present() const = 0;
		
		virtual uint32_t GetCurrentFrameIndex() const = 0;
		virtual const SwapChainSpecification& GetSpecification() = 0;
		virtual const SwapChainSpecification& GetSpecification() const = 0;
	};

}
