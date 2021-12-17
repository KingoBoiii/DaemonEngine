#pragma once
#include "DaemonEngine/Renderer/Texture2D.h"

#include <d3d11.h>

namespace Daemon
{

	class DX11Texture2D : public Texture2D
	{
	public:
		DX11Texture2D(const std::string& filepath, const TextureSpecification& specification);
		DX11Texture2D(uint32_t width, uint32_t height, const TextureSpecification& specification);
		DX11Texture2D(uint32_t width, uint32_t height, void* data, const TextureSpecification& specification);
		virtual ~DX11Texture2D();

		virtual void SetData(void* data) const override;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind() const override;

		virtual void* GetTexture() const override { return m_TextureView; }

		virtual bool operator==(const Texture2D& other) const override
		{
			KE_CORE_ASSERT(&other, "Cannot compare other texture!");
			return m_Texture == ((DX11Texture2D&)other).m_Texture && m_TextureView == ((DX11Texture2D&)other).m_TextureView;
		}
	private:
		void CreateTexture2D(DXGI_FORMAT format, int32_t channels, const void* data);
		void CreateSamplerState();
		void CreateShaderResourceView(uint32_t mipLevels);
	private:
		TextureSpecification m_Specification;
		int32_t m_Width;
		int32_t m_Height;

		ID3D11Texture2D* m_Texture;
		ID3D11ShaderResourceView* m_TextureView;
		ID3D11SamplerState* m_SamplerState;
	};

}