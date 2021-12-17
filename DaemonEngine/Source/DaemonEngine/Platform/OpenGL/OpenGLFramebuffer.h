#pragma once
#include "DaemonEngine/Renderer/Framebuffer.h"

namespace Daemon 
{

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& specification);
		~OpenGLFramebuffer();

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void Clear();
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual int ReadPixel(uint32_t attachmentIndex, int32_t x, int32_t y) override;

		virtual void Bind() const override;
		virtual void Unbind() override;

		virtual void* GetColorAttachment(uint32_t colorAttachmentIndex) const override { return (void*)m_ColorAttachments[colorAttachmentIndex]; }

		virtual const FramebufferSpecification& GetSpecification() override { return m_Specification; }
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		void Invalidate();
	private:
		FramebufferSpecification m_Specification;
		uint32_t m_RendererID;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};

}
