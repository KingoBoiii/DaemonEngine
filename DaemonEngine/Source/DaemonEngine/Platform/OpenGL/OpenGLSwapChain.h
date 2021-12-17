#pragma once
#include "DaemonEngine/Renderer/SwapChain.h"

namespace Daemon
{

	class OpenGLSwapChain : public SwapChain 
	{
	public:
		OpenGLSwapChain(const SwapChainSpecification& specification);
		virtual ~OpenGLSwapChain() = default;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void Present() const override;

		virtual uint32_t GetCurrentFrameIndex() const override { return 0; }
		virtual const SwapChainSpecification& GetSpecification() override { return m_Specification; }
		virtual const SwapChainSpecification& GetSpecification() const override { return m_Specification; }
	private:
		SwapChainSpecification m_Specification;
	};

}
