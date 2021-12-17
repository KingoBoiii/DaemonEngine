#pragma once
#include <glm/glm.hpp>

namespace Daemon 
{

	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,

		// Depth/stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format)
		{
		}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments)
		{
		}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width;
		uint32_t Height;
		FramebufferAttachmentSpecification Attachments;
		glm::vec4 ClearColor = { 0.2f, 0.2f, 0.2f, 0.2f };
	};

	class Framebuffer
	{
	public:
		static Shared<Framebuffer> Create(const FramebufferSpecification& specification = FramebufferSpecification());
	public:
		virtual ~Framebuffer() = default;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void Clear() = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual int ReadPixel(uint32_t attachmentIndex, int32_t x, int32_t y) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() = 0;

		virtual void* GetColorAttachment(uint32_t colorAttachmentIndex) const = 0;

		virtual const FramebufferSpecification& GetSpecification() = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;
	};

}
