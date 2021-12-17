#include "kepch.h"
#include "DX12Renderer.h"

#include "DX12Context.h"
#include "DX12Pipeline.h"
#include "DX12VertexBuffer.h"
#include "DX12IndexBuffer.h"
#include "DX12SwapChain.h"
#include "DX12UniformBuffer.h"
#include "DX12Texture2D.h"
#include "DaemonEngine/Platform/Win32/Win32Window.h"

namespace Daemon
{

	DX12Renderer::DX12Renderer()
	{
	}

	DX12Renderer::~DX12Renderer()
	{
	}

	void DX12Renderer::Init()
	{
        int width = 1600;
        int height = 900;
        m_Viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height) };
        m_ScissorRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

        CreateCommandAllocator();
        CreateCommandList();
    }

	void DX12Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
	{
	}

#if 0
	void DX12Renderer::Present() const
	{
        Win32Window* window = reinterpret_cast<Win32Window*>(Window::GetNativeWindow());
        DX12SwapChain* swapChain = reinterpret_cast<DX12SwapChain*>(window->GetSwapChain());

        swapChain->WaitForPreviousFrame();

        // Execute the command list.
        //ID3D12CommandList* ppCommandLists[] = { m_CommandList };
        //swapChain->GetD3DX12CommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        swapChain->GetD3DX12CommandQueue()->ExecuteCommandLists(static_cast<uint32_t>(m_CommandLists.size()), m_CommandLists.data());

        swapChain->Present();
	}
#endif

    void DX12Renderer::BeginFrame()
    {
#if 0
        Win32Window* window = reinterpret_cast<Win32Window*>(Window::GetNativeWindow());
        DX12SwapChain* swapChain = reinterpret_cast<DX12SwapChain*>(window->GetSwapChain());

        // Command list allocators can only be reset when the associated 
        // command lists have finished execution on the GPU; apps should use 
        // fences to determine GPU execution progress.
        m_CommandAllocator->Reset();

        // However, when ExecuteCommandList() is called on a particular command 
        // list, that command list can then be reset at any time and must be before 
        // re-recording.
        m_CommandList->Reset(m_CommandAllocator, nullptr);

        // Indicate that the back buffer will be used as a render target.
        D3D12_RESOURCE_BARRIER resourceBarrier;
        ZeroMemory(&resourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
        resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        resourceBarrier.Transition.pResource = swapChain->GetD3DX12RTVForCurrentFrame(); 
        resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        m_CommandList->ResourceBarrier(1, &resourceBarrier);
#endif
    }

    void DX12Renderer::EndFrame()
    {
#if 0
        Win32Window* window = reinterpret_cast<Win32Window*>(Window::GetNativeWindow());
        DX12SwapChain* swapChain = reinterpret_cast<DX12SwapChain*>(window->GetSwapChain());

        // Indicate that the back buffer will now be used to present.
        D3D12_RESOURCE_BARRIER resourceBarrier;
        ZeroMemory(&resourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
        resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        resourceBarrier.Transition.pResource = swapChain->GetD3DX12RTVForCurrentFrame(); 
        resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        m_CommandList->ResourceBarrier(1, &resourceBarrier);

        m_CommandList->Close();
        m_CommandLists.push_back(m_CommandList);
#endif
    }

    void DX12Renderer::RenderMesh(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<Mesh> mesh, const glm::mat4& transform)
    {
    }

    void DX12Renderer::RenderGeometry(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<VertexBuffer> vertexBuffer, Shared<IndexBuffer> indexBuffer, uint32_t indexCount)
	{
		if (indexCount == 0)
		{
			indexCount = indexBuffer->GetCount();
		}

        DX12Pipeline* dxPipeline = reinterpret_cast<DX12Pipeline*>(pipeline.get());
        ID3D12RootSignature* rootSignature = dxPipeline->GetD3DX12RootSignature();
        ID3D12PipelineState* pipelineState = dxPipeline->GetD3DX12PipelineState();
        m_CommandList->SetPipelineState(pipelineState);

        // Set necessary state.
        m_CommandList->SetGraphicsRootSignature(rootSignature);
        m_CommandList->RSSetViewports(1, &m_Viewport);
        m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

        // Set Descriptor heaps / bind uniform buffer
        DX12UniformBuffer* dxUniformBuffer = reinterpret_cast<DX12UniformBuffer*>(uniformBuffer.get());
        ID3D12DescriptorHeap* heaps[] = { dxUniformBuffer->m_ConstantBufferHeap };
        m_CommandList->SetDescriptorHeaps(_countof(heaps), heaps);
        m_CommandList->SetGraphicsRootDescriptorTable(0, dxUniformBuffer->m_ConstantBufferHeap->GetGPUDescriptorHandleForHeapStart());

        m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // Bind vertex buffer
        DX12VertexBuffer* dxVertexBuffer = reinterpret_cast<DX12VertexBuffer*>(vertexBuffer.get());
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView = dxVertexBuffer->GetVertexBufferView();
        m_CommandList->IASetVertexBuffers(0, 1, &vertexBufferView);

        // Bind index buffer
        DX12IndexBuffer* dxIndexBuffer = reinterpret_cast<DX12IndexBuffer*>(indexBuffer.get());
        D3D12_INDEX_BUFFER_VIEW indexBufferView = dxIndexBuffer->GetIndexBufferView();
        m_CommandList->IASetIndexBuffer(&indexBufferView);

        m_CommandList->DrawIndexedInstanced(indexBuffer->GetCount(), 1, 0, 0, 0);
	}

    void DX12Renderer::Clear(uint32_t buffer, const glm::vec4& clearColorRGBA) const
    {
#if 0
        Win32Window* window = reinterpret_cast<Win32Window*>(Window::GetNativeWindow());
        DX12SwapChain* swapChain = reinterpret_cast<DX12SwapChain*>(window->GetSwapChain());

        // Render View Target
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain->m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr = SIZE_T(INT64(rtvHandle.ptr) + INT64(swapChain->m_FrameIndex) * INT64(swapChain->m_RTVDescriptorSize));

        // Depth Stencil
        //D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = swapChain->m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
        //dsvHandle.ptr = SIZE_T(INT64(dsvHandle.ptr) + INT64(swapChain->m_FrameIndex) * INT64(swapChain->m_DSVDescriptorSize));
        m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        // Record commands.
        m_CommandList->ClearRenderTargetView(rtvHandle, &clearColorRGBA.r, 0, nullptr);
        //m_CommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0.0f, 0, nullptr);
#endif
    }

    void DX12Renderer::CreateCommandAllocator()
    {
        ID3D12Device8* device = DX12Context::Get().GetDevice();
        HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
        KE_CORE_ASSERT(!FAILED(hr), "Failed to create Command Allocator");
    }

    void DX12Renderer::CreateCommandList()
    {
        ID3D12Device8* device = DX12Context::Get().GetDevice();
        HRESULT hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator, nullptr, IID_PPV_ARGS(&m_CommandList));
        KE_CORE_ASSERT(!FAILED(hr), "Failed to create Command List");

        // When the CommandList is created, it starts in "Recording" state, so close it
        m_CommandList->Close();
    }

}