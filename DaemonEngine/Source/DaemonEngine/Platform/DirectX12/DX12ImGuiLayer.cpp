#include "kepch.h"
#include "DX12ImGuiLayer.h"
#include "DX12Context.h"

#include "DaemonEngine/Core/Application.h"
#include "DaemonEngine/Platform/Win32/Win32Window.h"

#include <Windows.h>

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx12.h>

namespace Daemon
{

	void DX12ImGuiLayer::OnAttach()
	{
		SetupDirectX12ForImGui();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", 18.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/consola.ttf", 16.0f);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		Win32Window* window = static_cast<Win32Window*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplWin32_Init(window->GetHwnd());
		DX12Context& context = DX12Context::Get();
		ImGui_ImplDX12_Init(context.GetDevice(), 2, DXGI_FORMAT_R8G8B8A8_UNORM, m_pd3dSrvDescHeap, m_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), m_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
	}

	void DX12ImGuiLayer::OnDetach()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void DX12ImGuiLayer::SetupDirectX12ForImGui()
	{
		DX12Context context = DX12Context::Get();
		ID3D12Device8* device = context.GetDevice();

		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC1 sd;
		{
			ZeroMemory(&sd, sizeof(sd));
			sd.BufferCount = NUM_BACK_BUFFERS;
			sd.Width = 0;
			sd.Height = 0;
			sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			sd.Scaling = DXGI_SCALING_STRETCH;
			sd.Stereo = FALSE;
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = 2;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 1;
			HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pd3dRtvDescHeap));
			KE_CORE_ASSERT(!FAILED(hr), "");

			SIZE_T rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
			for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
			{
				m_mainRenderTargetDescriptor[i] = rtvHandle;
				rtvHandle.ptr += rtvDescriptorSize;
			}
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = 1;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pd3dSrvDescHeap));
			KE_CORE_ASSERT(!FAILED(hr), "");
		}

		{
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 1;
			HRESULT hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pd3dCommandQueue));
			KE_CORE_ASSERT(!FAILED(hr), "");
		}

		for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		{
			HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_frameContext[i].CommandAllocator));
			KE_CORE_ASSERT(!FAILED(hr), "");
		}

		HRESULT hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&m_pd3dCommandList));
		KE_CORE_ASSERT(!FAILED(hr), "");
		m_pd3dCommandList->Close();

		hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
		KE_CORE_ASSERT(!FAILED(hr), "");

		m_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		KE_CORE_ASSERT(m_fenceEvent != NULL, "");

		{
			IDXGIFactory4* dxgiFactory = NULL;
			IDXGISwapChain1* swapChain1 = NULL;
			HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
			KE_CORE_ASSERT(!FAILED(hr), "Failed to create DXGI factory");

			Win32Window* window = reinterpret_cast<Win32Window*>(Window::GetNativeWindow());
			HWND hwnd = window->GetHwnd();
			hr = dxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, hwnd, &sd, NULL, NULL, &swapChain1);
			KE_CORE_ASSERT(!FAILED(hr), "Failed to create SwapChain for HWND");

			hr = swapChain1->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
			KE_CORE_ASSERT(!FAILED(hr), "");
			swapChain1->Release();
			dxgiFactory->Release();
			m_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
			m_hSwapChainWaitableObject = m_pSwapChain->GetFrameLatencyWaitableObject();
		}

		CreateRenderTarget(device);
	}

	void DX12ImGuiLayer::CreateRenderTarget(ID3D12Device8* device)
	{
		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			ID3D12Resource* pBackBuffer = NULL;
			m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			device->CreateRenderTargetView(pBackBuffer, NULL, m_mainRenderTargetDescriptor[i]);
			m_mainRenderTargetResource[i] = pBackBuffer;
		}
	}

	DX12ImGuiLayer::FrameContext* DX12ImGuiLayer::WaitForNextFrameResources()
	{
		UINT nextFrameIndex = m_frameIndex + 1;
		m_frameIndex = nextFrameIndex;

		HANDLE waitableObjects[] = { m_hSwapChainWaitableObject, NULL };
		DWORD numWaitableObjects = 1;

		FrameContext* frameCtx = &m_frameContext[nextFrameIndex % 2];
		UINT64 fenceValue = frameCtx->FenceValue;
		if (fenceValue != 0) // means no fence was signaled
		{
			frameCtx->FenceValue = 0;
			m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
			waitableObjects[1] = m_fenceEvent;
			numWaitableObjects = 2;
		}

		WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

		return frameCtx;
	}

	void DX12ImGuiLayer::Begin()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void DX12ImGuiLayer::End()
	{
		ImGui::EndFrame();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();

		FrameContext* frameCtx = WaitForNextFrameResources();
		UINT backBufferIdx = m_pSwapChain->GetCurrentBackBufferIndex();
		frameCtx->CommandAllocator->Reset();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_mainRenderTargetResource[backBufferIdx];
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_pd3dCommandList->Reset(frameCtx->CommandAllocator, NULL);
		m_pd3dCommandList->ResourceBarrier(1, &barrier);

		// Render Dear ImGui graphics
		const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		m_pd3dCommandList->ClearRenderTargetView(m_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, NULL);
		m_pd3dCommandList->OMSetRenderTargets(1, &m_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
		m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dSrvDescHeap);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_pd3dCommandList);
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		m_pd3dCommandList->ResourceBarrier(1, &barrier);
		m_pd3dCommandList->Close();

		m_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&m_pd3dCommandList);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			HGLRC backup_current_context = wglGetCurrentContext();

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, (void*)m_pd3dCommandList);

			wglMakeCurrent(wglGetCurrentDC(), backup_current_context);
		}

		m_pSwapChain->Present(1, 0); // Present with vsync
		//m_pSwapChain->Present(0, 0); // Present without vsync

		UINT64 fenceValue = m_fenceLastSignaledValue + 1;
		m_pd3dCommandQueue->Signal(m_fence, fenceValue);
		m_fenceLastSignaledValue = fenceValue;
		frameCtx->FenceValue = fenceValue;
	}

}