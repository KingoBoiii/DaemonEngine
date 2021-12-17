#include "kepch.h"
#include "DX11Texture2D.h"
#include "DX11Context.h"

#include "stb_image.h"
#include <math.h>

namespace Daemon
{

	namespace Utils
	{

		int32_t KingoTextureChannelsByFormat(TextureFormat format)
		{
			switch (format)
			{
				case TextureFormat::RGBA:				return 4;
				case TextureFormat::RGB:				return 3;
				case TextureFormat::RED:				return 4;
				case TextureFormat::RED_INTEGER:		return 4;
				case TextureFormat::LUMINANCE_ALPHA:	return 4;
				case TextureFormat::NONE:
				default: break;
			}
			return 0;
		}

		DXGI_FORMAT KingoTextureFormatToDX(TextureFormat format)
		{
			switch (format)
			{
				case TextureFormat::RGBA:				return DXGI_FORMAT_R8G8B8A8_UNORM;
				case TextureFormat::RGB:				return DXGI_FORMAT_R8G8B8A8_UNORM;
				case TextureFormat::RED:				return DXGI_FORMAT_R8G8B8A8_UNORM;
				case TextureFormat::RED_INTEGER:		return DXGI_FORMAT_R8G8B8A8_UNORM;
				case TextureFormat::LUMINANCE_ALPHA:	return DXGI_FORMAT_R8G8B8A8_UNORM;
				case TextureFormat::NONE:
				default: break;
			}
			return DXGI_FORMAT_UNKNOWN;
		}

		D3D11_FILTER KingoFilterToDX(TextureFilter filter)
		{
			switch (filter)
			{
				case TextureFilter::NEAREST: return D3D11_FILTER_MIN_MAG_MIP_POINT;
				case TextureFilter::LINEAR: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				case TextureFilter::NONE:
				default: break;
			}
			return D3D11_FILTER_ANISOTROPIC;
		}

		D3D11_TEXTURE_ADDRESS_MODE KingoTextureWrapToDX(TextureWrap wrap)
		{
			switch (wrap)
			{
				case TextureWrap::CLAMP:				return D3D11_TEXTURE_ADDRESS_CLAMP;	
				case TextureWrap::CLAMP_TO_BORDER:		return D3D11_TEXTURE_ADDRESS_CLAMP;
				case TextureWrap::CLAMP_TO_EDGE:		return D3D11_TEXTURE_ADDRESS_CLAMP;
				case TextureWrap::REPEAT:				return D3D11_TEXTURE_ADDRESS_WRAP;
				case TextureWrap::MIRRORED_REPEAT:		return D3D11_TEXTURE_ADDRESS_MIRROR;
				case TextureWrap::NONE:
				default: break;
			}
			return D3D11_TEXTURE_ADDRESS_WRAP;
		}

	}

	DX11Texture2D::DX11Texture2D(const std::string& filepath, const TextureSpecification& specification)
		: m_Specification(specification)
	{
		int32_t channels;
		stbi_set_flip_vertically_on_load(m_Specification.FlipVertically);
		unsigned char* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &channels, 0);
		if (data)
		{
			m_Specification.Format = channels == 4 ? TextureFormat::RGBA : TextureFormat::RGB;
			CreateTexture2D(Utils::KingoTextureFormatToDX(m_Specification.Format), channels, data);
			stbi_image_free(data);
		}
	}

	DX11Texture2D::DX11Texture2D(uint32_t width, uint32_t height, const TextureSpecification& specification)
		: m_Width(width), m_Height(height), m_Specification(specification)
	{
		int32_t channels = Utils::KingoTextureChannelsByFormat(m_Specification.Format);
		CreateTexture2D(Utils::KingoTextureFormatToDX(m_Specification.Format), channels, nullptr);
	}

	DX11Texture2D::DX11Texture2D(uint32_t width, uint32_t height, void* data, const TextureSpecification& specification)
		: m_Width(width), m_Height(height), m_Specification(specification)
	{
		int32_t channels = Utils::KingoTextureChannelsByFormat(m_Specification.Format);
		CreateTexture2D(Utils::KingoTextureFormatToDX(m_Specification.Format), channels, data);
	}

	DX11Texture2D::~DX11Texture2D()
	{
		ReleaseCOM(m_Texture);
		ReleaseCOM(m_TextureView);
		ReleaseCOM(m_SamplerState);
	}

	void DX11Texture2D::SetData(void* data) const
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		memset(&msr, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

		DX11Context& context = DX11Context::Get();
		context.GetDeviceContext()->Map(m_Texture, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msr);
		memcpy(msr.pData, data, sizeof(data));
		context.GetDeviceContext()->Unmap(m_Texture, NULL);
	}

	void DX11Texture2D::Bind(uint32_t slot) const
	{
		DX11Context& context = DX11Context::Get();
		context.GetDeviceContext()->PSSetShaderResources(slot, 1, &m_TextureView);
		context.GetDeviceContext()->PSSetSamplers(slot, 1, &m_SamplerState);
	}

	void DX11Texture2D::Unbind() const
	{
	}

	void DX11Texture2D::CreateTexture2D(DXGI_FORMAT format, int32_t channels, const void* data)
	{
		D3D11_TEXTURE2D_DESC desc;
		memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
		bool generateMips = data != nullptr;
		uint32_t mipLevels = 1;

		if (generateMips)
		{
			uint32_t width = m_Width;
			uint32_t height = m_Height;

			while (width > 1 && height > 1)
			{
				width = std::max(width / 2, 1u);
				height = std::max(height / 2, 1u);
				++mipLevels;
			}
		}

		desc.Width = m_Width;
		desc.Height = m_Height;
		desc.MipLevels = mipLevels;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = generateMips ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (generateMips)
		{
			desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		}
		desc.CPUAccessFlags = generateMips ? 0 : D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

		DX11Context& context = DX11Context::Get();
		HRESULT hr = context.GetDevice()->CreateTexture2D(&desc, nullptr, &m_Texture);
		KE_CORE_ASSERT(!FAILED(hr) && m_Texture, "Failed to create Texture2D");

		CreateShaderResourceView(desc.MipLevels);
		if (generateMips)
		{
			int rowPitch = (m_Width * channels) * sizeof(unsigned char);
			context.GetDeviceContext()->UpdateSubresource(m_Texture, 0, nullptr, data, rowPitch, 0);
			context.GetDeviceContext()->GenerateMips(m_TextureView);
		}

		CreateSamplerState();
	}

	void DX11Texture2D::CreateSamplerState()
	{
		D3D11_SAMPLER_DESC desc;
		memset(&desc, 0, sizeof(D3D11_SAMPLER_DESC));

		desc.AddressU = Utils::KingoTextureWrapToDX(m_Specification.WrapMode);
		desc.AddressV = Utils::KingoTextureWrapToDX(m_Specification.WrapMode);
		desc.AddressW = Utils::KingoTextureWrapToDX(m_Specification.WrapMode);
		desc.MinLOD = 0;
		desc.MaxLOD = 11;
		desc.Filter = Utils::KingoFilterToDX(m_Specification.Filter);
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		DX11Context& context = DX11Context::Get();
		HRESULT hr = context.GetDevice()->CreateSamplerState(&desc, &m_SamplerState);
		KE_CORE_ASSERT(!FAILED(hr) && m_SamplerState, "Failed to create Sampler State for Texture2D");
	}

	void DX11Texture2D::CreateShaderResourceView(uint32_t mipLevels)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		memset(&desc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		desc.Format = Utils::KingoTextureFormatToDX(m_Specification.Format);
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 1;
		desc.Texture2DArray.MipLevels = mipLevels;

		DX11Context& context = DX11Context::Get();
		HRESULT hr = context.GetDevice()->CreateShaderResourceView(m_Texture, &desc, &m_TextureView);
		KE_CORE_ASSERT(!FAILED(hr) && m_Texture, "Failed to create Shader Resource View for Texture2D");
	}

}