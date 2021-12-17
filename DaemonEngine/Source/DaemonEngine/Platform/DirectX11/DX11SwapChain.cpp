#include "kepch.h"
#include "DX11SwapChain.h"
#include "DX11Context.h"

namespace Daemon
{

#define EXPERIMENTAL_SWAP_CHAIN_EFFECT 0

	DX11SwapChain::DX11SwapChain(const SwapChainSpecification& specification)
		: m_Specification(specification)
	{
		DX11Context context = DX11Context::Get();
		ID3D11Device* device = context.GetDevice();
		ID3D11DeviceContext* deviceContext = context.GetDeviceContext();

		HWND hwnd = reinterpret_cast<HWND>(m_Specification.WindowHandle);
		CreateSwapChain(hwnd, device, deviceContext);

		CreateRenderTargetView(device);
		CreateDepthStencilView(device);
		SetRenderTargets(m_RenderTargetView);
		ResizeViewport(0, 0, m_Specification.Width, m_Specification.Height);

		CreateFence(device);
		Wait();
	}

	DX11SwapChain::~DX11SwapChain()
	{
		ReleaseCOM(m_SwapChain);
		ReleaseCOM(m_Fence);
		ReleaseCOM(m_RenderTargetView);
		ReleaseCOM(m_DepthStencilView);
	}

	void DX11SwapChain::Present() const
	{
		m_SwapChain->Present(1, 0);
	}

	void DX11SwapChain::Resize(uint32_t width, uint32_t height)
	{
		DX11Context context = DX11Context::Get();
		ID3D11Device4* device = context.GetDevice();
		ID3D11DeviceContext4* deviceContext = context.GetDeviceContext();
		deviceContext->Flush();
		deviceContext->ClearState();

		m_Specification.Width = width;
		m_Specification.Height = height;
		if (m_Specification.Width <= 0 || m_Specification.Height <= 0)
		{
			return;
		}

		ReleaseCOM(m_SwapChain);
		ReleaseCOM(m_RenderTargetView);
		ReleaseCOM(m_DepthStencilView);

		HWND hwnd = reinterpret_cast<HWND>(m_Specification.WindowHandle);
		CreateSwapChain(hwnd, device, deviceContext);
		m_SwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_UNKNOWN, 0);

		CreateRenderTargetView(device);
		CreateDepthStencilView(device);
		SetRenderTargets(m_RenderTargetView);
		ResizeViewport(0, 0, width, height);

		Wait();
	}

	void DX11SwapChain::ResizeViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		DX11Context context = DX11Context::Get();
		ID3D11DeviceContext* deviceContext = context.GetDeviceContext();

		ZeroMemory(&m_Viewport, sizeof(D3D11_VIEWPORT));
		m_Viewport.TopLeftX = static_cast<float>(x);
		m_Viewport.TopLeftY = static_cast<float>(y);
		m_Viewport.Width = static_cast<float>(width);
		m_Viewport.Height = static_cast<float>(height);
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
		deviceContext->RSSetViewports(1, &m_Viewport);
	}

	void DX11SwapChain::SetRenderTargets(ID3D11RenderTargetView* targets)
	{
		DX11Context context = DX11Context::Get();
		ID3D11DeviceContext* deviceContext = context.GetDeviceContext();
		deviceContext->OMSetRenderTargets(1, &targets, m_DepthStencilView);
	}

	void DX11SwapChain::SetRenderTargets(uint32_t count, ID3D11RenderTargetView* const* targets)
	{
		DX11Context context = DX11Context::Get();
		ID3D11DeviceContext* deviceContext = context.GetDeviceContext();
		deviceContext->OMSetRenderTargets(count, targets, m_DepthStencilView);
	}

	void DX11SwapChain::ResetRenderTargets()
	{
		DX11Context context = DX11Context::Get();
		ID3D11DeviceContext* deviceContext = context.GetDeviceContext();
		deviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
	}

	void DX11SwapChain::Wait()
	{
		ID3D11DeviceContext4* deviceContext = DX11Context::Get().GetDeviceContext();
		deviceContext->Signal(m_Fence, m_FenceValue);

		HRESULT hr = m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to set completion event for Fence");
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
		m_FenceValue++;
	}

	void DX11SwapChain::WaitForPreviousFrame()
	{
		ID3D11DeviceContext4* deviceContext = DX11Context::Get().GetDeviceContext();

		const uint64_t currentFenceValue = m_FenceValue;
		HRESULT hr = deviceContext->Signal(m_Fence, currentFenceValue);

		if (m_Fence->GetCompletedValue() < m_FenceValue)
		{
			HRESULT hr = m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
			KE_CORE_ASSERT(!FAILED(hr), "Failed to set completion event for Fence");
			WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
		}
		m_FenceValue = currentFenceValue + 1;
	}

	void DX11SwapChain::CreateSwapChain(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
#if EXPERIMENTAL_SWAP_CHAIN_EFFECT
		desc.BufferCount = 2;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
#else
		desc.BufferCount = 1;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
#endif
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferDesc.Width = m_Specification.Width;
		desc.BufferDesc.Height = m_Specification.Height;
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.OutputWindow = hwnd;
		desc.Windowed = true;


		IDXGIFactory* factory;
		HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&factory));
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create DX11 Factory");

		hr = factory->CreateSwapChain(device, &desc, &m_SwapChain);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create DirectX 11 SwapChain");
	}

	void DX11SwapChain::CreateRenderTargetView(ID3D11Device* device)
	{
		ID3D11Texture2D* texture;
		HRESULT hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&texture);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to get buffer");

		hr = device->CreateRenderTargetView(texture, nullptr, &m_RenderTargetView);
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create Render Target View");
		texture->Release();
	}

	void DX11SwapChain::CreateDepthStencilView(ID3D11Device* device)
	{
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
		KE_CORE_ASSERT(!FAILED(hr) && depthStencilBuffer, "Failed to create Depth Stencil Buffer");

		hr = device->CreateDepthStencilView(depthStencilBuffer, 0, &m_DepthStencilView);
		KE_CORE_ASSERT(!FAILED(hr) && m_DepthStencilView, "Failed to create Depth Stencil View");

		depthStencilBuffer->Release();
	}

	void DX11SwapChain::CreateFence(ID3D11Device* device)
	{
		ID3D11Device5* device5 = reinterpret_cast<ID3D11Device5*>(device);
		HRESULT hr = device5->CreateFence(m_FenceValue, D3D11_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
		KE_CORE_ASSERT(!FAILED(hr), "Failed to create Fence");

		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		KE_CORE_ASSERT(m_FenceEvent, ("Failed to create event {0}", HRESULT_FROM_WIN32(GetLastError())));
	}

}