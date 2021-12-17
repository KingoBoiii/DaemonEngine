#pragma once
#include "DaemonEngine/Renderer/Framebuffer.h"
#include "DX11Shader.h"
#include "DX11SwapChain.h"

namespace Daemon
{

	class DX11Framebuffer : public Framebuffer
	{
	public:
		DX11Framebuffer(const FramebufferSpecification& specification);
		virtual ~DX11Framebuffer();

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void Clear();
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual int ReadPixel(uint32_t attachmentIndex, int32_t x, int32_t y) override;

		virtual void Bind() const override;
		virtual void Unbind() override;

		virtual void* GetColorAttachment(uint32_t colorAttachmentIndex) const override;

		virtual const FramebufferSpecification& GetSpecification() override { return m_Specification; }
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		void Invalidate();

		void CreateStagingTexture();
		void CreateColorAttachment(size_t index, FramebufferTextureSpecification spec);
		void CreateDepthStencilAttachment();
	private:
		FramebufferSpecification m_Specification;
		DX11SwapChain* m_SwapChain;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		ID3D11DepthStencilView* m_FramebufferDepth = nullptr;
		std::vector<ID3D11RenderTargetView*> m_FramebufferViews = std::vector<ID3D11RenderTargetView*>();
		std::vector<ID3D11Texture2D*> m_FramebufferTextures = std::vector<ID3D11Texture2D*>();
		std::vector<ID3D11ShaderResourceView*> m_ShaderResourceViews = std::vector<ID3D11ShaderResourceView*>(); // Color attachments
	};

}
