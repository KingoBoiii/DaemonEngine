#pragma once
#include "Core/Application.h"
#include "Core/Log.h"
#include "Renderer/RendererAPI.h"

extern Daemon::Application* Daemon::CreateApplication();
bool g_ApplicationRunning = true;

int main(int argc, char** argv)
{
	while (g_ApplicationRunning)
	{
		Daemon::InitializeCore();

		Daemon::Application* app = Daemon::CreateApplication();
		app->Run();
		delete app;

		Daemon::ShutdownCore();
	}
}
