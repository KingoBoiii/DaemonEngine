#pragma once
#include "DaemonEngine/Renderer/Texture2D.h"

namespace Daemon
{

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& filepath, const TextureSpecification& specification = TextureSpecification());
		OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSpecification& specification = TextureSpecification());
		OpenGLTexture2D(uint32_t width, uint32_t height, void* data, const TextureSpecification& specification = TextureSpecification());
		~OpenGLTexture2D();

		virtual void SetData(void* data) const override;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind() const override;

		virtual void* GetTexture() const override { return (void*)m_RendererID; }

		virtual bool operator==(const Texture2D& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}
	private:
		void SetTextureParameters();
	private:
		uint32_t m_RendererID;
		int32_t m_Width;
		int32_t m_Height;
		TextureSpecification m_Specification;
	};

}
