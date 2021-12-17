#include "kepch.h"
#include "DX12SwapChain.h"
#include "DX12Context.h"

namespace Daemon
{

    DX12SwapChain::DX12SwapChain(HWND handle)
    {
        RECT rect;
        KE_CORE_ASSERT(GetWindowRect(handle, &rect), "Cannot get window size...");
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
        swapChainDesc.BufferCount = FrameCount;
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ID3D12Device8* device = DX12Context::Get().GetDevice();
        CreateCommandQueue(device);

        IDXGIFactory4* factory;
        CreateDXGIFactory(IID_PPV_ARGS(&factory));
        IDXGISwapChain1* swapChain;
        HRESULT hr = factory->CreateSwapChainForHwnd(m_CommandQueue, handle, &swapChainDesc, nullptr, nullptr, &swapChain);
        m_SwapChain = static_cast<IDXGISwapChain3*>(swapChain);
        KE_CORE_ASSERT(!FAILED(hr) && m_SwapChain, "Failed to create a Swap Chain");

        //hr = factory->MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER);
        //KE_CORE_ASSERT(!FAILED(hr), "Failed to make window association");

        CreateRenderViewTarget(device);
        CreateFence(device);
        //CreateDepthStencilViewTarget(device);
    }

    DX12SwapChain::~DX12SwapChain()
    {
        // Wait for the GPU to be done with all resources.
        WaitForPreviousFrame();
        CloseHandle(m_FenceEvent);
    }

    void DX12SwapChain::Present()
    {
        m_SwapChain->Present(1, 0);
        WaitForPreviousFrame();
    }

    void DX12SwapChain::WaitForPreviousFrame()
    {
        // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
        // This is code implemented as such for simplicity. More advanced samples 
        // illustrate how to use fences for efficient resource usage.

        // Signal and increment the fence value.
        const UINT64 fence = m_FenceValue;
        m_CommandQueue->Signal(m_Fence, fence);
        m_FenceValue++;

        // Wait until the previous frame is finished.
        if (m_Fence->GetCompletedValue() < fence)
        {
            m_Fence->SetEventOnCompletion(fence, m_FenceEvent);
            WaitForSingleObject(m_FenceEvent, INFINITE);
        }

        m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
    }

    void DX12SwapChain::CreateCommandQueue(ID3D12Device8* device)
    {
        D3D12_COMMAND_QUEUE_DESC cmdQueueDesc;
        ZeroMemory(&cmdQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
        cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        HRESULT hr = device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_CommandQueue));
        KE_CORE_ASSERT(!FAILED(hr), "Failed to create a Command Queue");
    }

    void DX12SwapChain::CreateRenderViewTarget(ID3D12Device8* device)
    {
        m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

        // Create descriptor heaps
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
        ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NumDescriptors = FrameCount;
        device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RTVHeap));
        m_RTVDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        // Create frame resources
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
        // Create Render Target View for each frame
        for (UINT n = 0; n < FrameCount; n++)
        {
            HRESULT hr = m_SwapChain->GetBuffer(n, IID_PPV_ARGS(&m_RenderTargets[n]));
            KE_CORE_ASSERT(!FAILED(hr) && m_RenderTargets[n], "Failed to create a get buffer from Swap Chain");
            device->CreateRenderTargetView(m_RenderTargets[n], nullptr, rtvHandle);
            rtvHandle.ptr = SIZE_T(INT64(rtvHandle.ptr) + INT64(1) * INT64(m_RTVDescriptorSize));
        }
    }

    void DX12SwapChain::CreateFence(ID3D12Device8* device)
    {
        HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
        KE_CORE_ASSERT(!FAILED(hr), "Failed to create Fence");
        m_FenceValue = 1;

        m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
        KE_CORE_ASSERT(m_FenceEvent != nullptr, (HRESULT_FROM_WIN32(GetLastError())));

        WaitForPreviousFrame();
    }

}