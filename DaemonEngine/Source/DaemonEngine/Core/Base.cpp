#include "kepch.h"
#include "Base.h"

#include "Log.h"
#include "DaemonEngine/System/MemoryManager.h"

#define KINGO_BUILD_ID "V: 0.0.200 - Development"

namespace Daemon
{

	void InitializeCore()
	{
		MemoryManager::Initialize();
		Log::Init();

		KE_CORE_TRACE("Daemon Engine {}", KINGO_BUILD_ID);
		KE_CORE_TRACE("Initializing...");
	}

	void ShutdownCore()
	{
		KE_CORE_TRACE("Shutting down...");

		Log::Shutdown();
		MemoryManager::Shutdown();
	}

}