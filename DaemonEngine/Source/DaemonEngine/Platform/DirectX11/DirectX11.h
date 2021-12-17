#pragma once

#include <d3d11_4.h>
#pragma comment(lib,"d3d11.lib")
#include <dxgi.h>
#pragma comment(lib,"dxgi.lib")

#include <d3dcompiler.h>

#include <Windows.h>

#define ReleaseCOM(x)	\
	if (x)				\
	{					\
		x->Release();	\
		x = nullptr;		\
	}