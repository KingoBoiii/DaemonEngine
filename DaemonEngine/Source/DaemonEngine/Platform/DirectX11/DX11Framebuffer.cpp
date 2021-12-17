#include "kepch.h"
#include "DX11Framebuffer.h"
#include "DX11Context.h"

#include "DaemonEngine/Renderer/Renderer.h"

#include "DaemonEngine/Core/Application.h"
#include "DaemonEngine/Platform/Win32/Win32Window.h"

namespace Daemon
{

	namespace Utils
	{

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
			}

			return false;
		}

		static DXGI_FORMAT KingoFBTextureFormatToD3DX11(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::RGBA8:       return DXGI_FORMAT_R8G8B8A8_UNORM;
				case FramebufferTextureFormat::RED_INTEGER: return DXGI_FORMAT_R32_SINT;
			}

			KE_CORE_ASSERT(false);
			return DXGI_FORMAT_UNKNOWN;
		}

	}

	DX11Framebuffer::DX11Framebuffer(const FramebufferSpecification& specification)
		: m_Specification(specification)
	{
		m_SwapChain = (DX11SwapChain*)Application::Get().GetWindow().GetSwapChain().get();

		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
			{
				m_ColorAttachmentSpecifications.emplace_back(spec);
			}
			else
			{
				m_DepthAttachmentSpecification = spec;
			}
		}

		Invalidate();
	}

	DX11Framebuffer::~DX11Framebuffer()
	{
		for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); i++)
		{
			ReleaseCOM(m_FramebufferTextures[i]);
			ReleaseCOM(m_FramebufferViews[i]);
			ReleaseCOM(m_ShaderResourceViews[i]);
		}
		m_FramebufferTextures.clear();
		m_FramebufferViews.clear();
		m_ShaderResourceViews.clear();
	}

	void DX11Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width <= 0 || height <= 0)
		{
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}

	void DX11Framebuffer::Clear()
	{
		DX11Context context = DX11Context::Get();
		ID3D11DeviceContext4* deviceContext = context.GetDeviceContext();

		for (size_t i = 0; i < m_FramebufferViews.size(); i++)
		{
			deviceContext->ClearRenderTargetView(m_FramebufferViews[i], &m_Specification.ClearColor[0]);
		}

		if (m_FramebufferDepth)
		{
			deviceContext->ClearDepthStencilView(m_FramebufferDepth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
		else
		{
			deviceContext->ClearDepthStencilView(m_SwapChain->GetD3DX11DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}

	void DX11Framebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		KE_CORE_ASSERT(m_ColorAttachmentSpecifications.size() > attachmentIndex);

		DX11Context context = DX11Context::Get();
		ID3D11DeviceContext4* deviceContext = context.GetDeviceContext();
		float clearColor[4] = { static_cast<float>(value), 0, 0, 0 };
		deviceContext->ClearRenderTargetView(m_FramebufferViews[attachmentIndex], clearColor);
	}

	int DX11Framebuffer::ReadPixel(uint32_t attachmentIndex, int32_t x, int32_t y)
	{
		KE_CORE_ASSERT(m_ColorAttachmentSpecifications.size() > attachmentIndex);

		DX11Context context = DX11Context::Get();
		ID3D11Device4* device = context.GetDevice();
		ID3D11DeviceContext4* deviceContext = context.GetDeviceContext();

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Format = Utils::KingoFBTextureFormatToD3DX11(m_ColorAttachmentSpecifications[attachmentIndex].TextureFormat);
		desc.Width = m_Specification.Width;
		desc.Height = m_Specification.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;
		ID3D11Texture2D* stagingTexture = nullptr;
		HRESULT hr = device->CreateTexture2D(&desc, NULL, &stagingTexture);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create staging texture");

		deviceContext->CopyResource(stagingTexture, m_FramebufferTextures[attachmentIndex]);

		D3D11_MAPPED_SUBRESOURCE msr;
		hr = deviceContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &msr);
		int offset = y * msr.RowPitch + x * 4;
		int32_t pixelData = *(const int32_t*)((const uint8_t*)msr.pData + offset);
		deviceContext->Unmap(stagingTexture, 0);

		ReleaseCOM(stagingTexture);
		return pixelData;
	}

	void DX11Framebuffer::Bind() const
	{
		DX11Context context = DX11Context::Get();
		ID3D11DeviceContext4* deviceContext = context.GetDeviceContext();
		deviceContext->OMSetRenderTargets(static_cast<uint32_t>(m_FramebufferViews.size()), m_FramebufferViews.data(), m_FramebufferDepth);

		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = static_cast<float>(0);
		viewport.TopLeftY = static_cast<float>(0);
		viewport.Width = static_cast<float>(m_Specification.Width);
		viewport.Height = static_cast<float>(m_Specification.Height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		deviceContext->RSSetViewports(1, &viewport);
	}

	void DX11Framebuffer::Unbind()
	{
		DX11Context context = DX11Context::Get();
		ID3D11Device4* device = context.GetDevice();

		for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); i++)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

			desc.Format = Utils::KingoFBTextureFormatToD3DX11(m_ColorAttachmentSpecifications[i].TextureFormat);
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = 1;

			ReleaseCOM(m_ShaderResourceViews[i]);
			HRESULT hr = device->CreateShaderResourceView(m_FramebufferTextures[i], &desc, &m_ShaderResourceViews[i]);
			KE_CORE_ASSERT(!FAILED(hr), "Failed to create SRV");
		}

		m_SwapChain->ResetRenderTargets();
	}

	void* DX11Framebuffer::GetColorAttachment(uint32_t colorAttachmentIndex) const
	{
		KE_CORE_ASSERT(m_ShaderResourceViews.size() > colorAttachmentIndex);
		return m_ShaderResourceViews[colorAttachmentIndex];
	}

	void DX11Framebuffer::Invalidate()
	{
		for (size_t i = 0; i < m_FramebufferTextures.size(); i++)
		{
			ReleaseCOM(m_FramebufferTextures[i]);
		}
		for (size_t i = 0; i < m_FramebufferViews.size(); i++)
		{
			ReleaseCOM(m_FramebufferViews[i]);
		}
		for (size_t i = 0; i < m_ShaderResourceViews.size(); i++)
		{
			ReleaseCOM(m_ShaderResourceViews[i]);
		}

		DX11Context context = DX11Context::Get();
		ID3D11Device* device = context.GetDevice();

		if (m_ColorAttachmentSpecifications.size())
		{
			m_FramebufferViews.resize(m_ColorAttachmentSpecifications.size());
			m_FramebufferTextures.resize(m_ColorAttachmentSpecifications.size());
			m_ShaderResourceViews.resize(m_ColorAttachmentSpecifications.size());

			for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); i++)
			{
				CreateColorAttachment(i, m_ColorAttachmentSpecifications[i]);
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			CreateDepthStencilAttachment();
		}
	}

	void DX11Framebuffer::CreateStagingTexture()
	{
	}

	void DX11Framebuffer::CreateColorAttachment(size_t index, FramebufferTextureSpecification spec)
	{
		DX11Context context = DX11Context::Get();
		ID3D11Device* device = context.GetDevice();

		bool red = spec.TextureFormat == FramebufferTextureFormat::RED_INTEGER;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.Format = Utils::KingoFBTextureFormatToD3DX11(spec.TextureFormat);
		desc.Width = m_Specification.Width;
		desc.Height = m_Specification.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.CPUAccessFlags = red ? D3D11_CPU_ACCESS_READ : 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		HRESULT hr = device->CreateTexture2D(&desc, NULL, &m_FramebufferTextures[index]);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create framebuffer texture");

		D3D11_RENDER_TARGET_VIEW_DESC desc1;
		ZeroMemory(&desc1, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		desc1.Format = Utils::KingoFBTextureFormatToD3DX11(m_ColorAttachmentSpecifications[index].TextureFormat);
		desc1.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = device->CreateRenderTargetView(m_FramebufferTextures[index], &desc1, &m_FramebufferViews[index]);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create framebuffer view");
	}

	void DX11Framebuffer::CreateDepthStencilAttachment()
	{
		ReleaseCOM(m_FramebufferDepth);

		DX11Context context = DX11Context::Get();
		ID3D11Device* device = context.GetDevice();
		ID3D11Texture2D* depthStencilBuffer;

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = m_Specification.Width;
		depthStencilDesc.Height = m_Specification.Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		HRESULT hr = device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create Depth Stencil Buffer");

		hr = device->CreateDepthStencilView(depthStencilBuffer, 0, &m_FramebufferDepth);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create Depth Stencil View");

		ReleaseCOM(depthStencilBuffer);
	}

}