#include "kepch.h"
#include "Win32Window.h"

#define NOMINMAX
#undef NOGDI
#include <Windows.h>
#include <Windowsx.h>
#define NOGDI
#include <tchar.h>

#include "DaemonEngine/Events/ApplicationEvent.h"
#include "DaemonEngine/Events/MouseEvent.h"
#include "DaemonEngine/Events/KeyEvent.h"
#include "DaemonEngine/Core/MouseCodes.h"

#include "DaemonEngine/Platform/DirectX12/DX12Context.h"

#include <backends/imgui_impl_opengl3.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace Daemon
{

	static const wchar_t* s_ClassName = L"DaemonEngine Win32 Window";

	static PIXELFORMATDESCRIPTOR GetPixelFormat()
	{
		PIXELFORMATDESCRIPTOR result = {};
		result.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		result.nVersion = 1;
		result.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		result.iPixelType = PFD_TYPE_RGBA;
		result.cColorBits = 32;
		result.cDepthBits = 24;
		result.cStencilBits = 8;
		result.cAuxBuffers = 0;
		result.iLayerType = PFD_MAIN_PLANE;
		return result;
	}

	Win32Window::Win32Window(const WindowSpecification& specification)
		: m_Specification(specification)
	{
		s_NativeWindowHandle = this;
	}

	Win32Window::~Win32Window()
	{
		DestroyWindow(hWnd);
		UnregisterClassW(s_ClassName, hInstance);
	}

	void Win32Window::Initialize()
	{
		hInstance = (HINSTANCE)&__ImageBase;

		WNDCLASSEX wclass = { 0 }; // Or: WNDCLASSEXW
		wclass.cbSize = sizeof(wclass);
		wclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wclass.lpfnWndProc = &WndProc;
		wclass.cbClsExtra = 0;
		wclass.cbWndExtra = 0;
		wclass.hInstance = hInstance;
		wclass.hIcon = NULL; // TODO: CREATE ICON
		wclass.hCursor = NULL;
		wclass.hbrBackground = NULL;//(HBRUSH)(COLOR_WINDOW+1);
		wclass.lpszMenuName = NULL;
		wclass.lpszClassName = s_ClassName;
		wclass.hIconSm = NULL;

		ATOM result = RegisterClassEx(&wclass);
		KE_CORE_ASSERT(result, "Failed to register Win32 class"); // Or: RegisterClassExW()

		std::wstring wTitle = std::wstring(m_Specification.Title.begin(), m_Specification.Title.end());
		hWnd = CreateWindowW( // Or: CreateWindowW()
							 s_ClassName,
							 wTitle.c_str(), // Or: L"NAME OF WINDOW"
							 WS_VISIBLE | WS_OVERLAPPEDWINDOW,
							 CW_USEDEFAULT,
							 CW_USEDEFAULT,
							 m_Specification.Width,//WIDTH:[TODO]->Make custom width to fit window
							 m_Specification.Height,//HEIGHT:[TODO]->Make custom width to fit window
							 0,
							 0,
							 hInstance,
							 0
		);
		KE_CORE_ASSERT(hWnd, "Failed to create Win32 window");

		hDc = GetDC(hWnd);
		PIXELFORMATDESCRIPTOR pfd = GetPixelFormat();
		int32_t pixelFormat = ChoosePixelFormat(hDc, &pfd);
		KE_CORE_ASSERT(pixelFormat, "Failed choosing the pixel format");
		SetPixelFormat(hDc, pixelFormat, &pfd);
		KE_CORE_ASSERT(pixelFormat, "Failed setting the pixel format");

		m_RendererContext = RendererContext::Create(&hWnd);
		m_RendererContext->Init();

		SwapChainSpecification spec = {};
		spec.Width = m_Data.Width;
		spec.Height = m_Data.Height;
		spec.WindowHandle = hWnd;
		m_SwapChain = SwapChain::Create(spec);

		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
	}

	void Win32Window::Update()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Win32Window::Present() const
	{
		m_SwapChain->Present();
	}

	void Win32Window::SetTitle(const std::string& title)
	{
		m_Specification.Title = title;
		std::wstring wTitle = std::wstring(title.begin(), title.end());
		SetWindowText(hWnd, wTitle.c_str());
	}

	void Win32Window::Maximize()
	{
		PostMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}

	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		{
			return true;
		}

		LRESULT result = NULL;
		Win32Window* window = (Win32Window*)Window::s_NativeWindowHandle;

		switch (message)
		{
			case WM_LBUTTONDOWN:
			{
				MouseButtonPressedEvent e(Mouse::ButtonLeft);
				window->m_Data.EventCallback(e);
			} break;
			case WM_MOUSEWHEEL:
			{
				int zDelta = GET_WHEEL_DELTA_WPARAM(wParam) / 100;
				MouseScrolledEvent e(0.0f, static_cast<float>(zDelta));
				window->m_Data.EventCallback(e);
			} break;
			case WM_SYSKEYUP:
			case WM_KEYUP:
			{
				uint32_t flags = static_cast<uint32_t>(lParam);
				uint32_t key = static_cast<uint32_t>(wParam);
				KeyReleasedEvent e(key);
				window->m_Data.EventCallback(e);
			} break;
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				uint32_t flags = static_cast<uint32_t>(lParam);
				uint32_t key = static_cast<uint32_t>(wParam);
				KeyPressedEvent e(key, (flags >> 30) & 1);
				window->m_Data.EventCallback(e);
			} break;
			case WM_SIZE:		// Window Resize event
			{
				window->m_Data.Width = LOWORD(lParam);
				window->m_Data.Height = HIWORD(lParam);

				WindowResizeEvent e(window->m_Data.Width, window->m_Data.Height);
				if (window->m_Data.EventCallback != nullptr)
				{
					window->m_Data.EventCallback(e);
				}
			} break;
			case WM_CLOSE:		// Window Close event
			{
				WindowCloseEvent e;
				window->m_Data.EventCallback(e);
				PostQuitMessage(0);
			} break;
			default: result = DefWindowProc(hWnd, message, wParam, lParam); break;
		}
		return result;
	}

}
