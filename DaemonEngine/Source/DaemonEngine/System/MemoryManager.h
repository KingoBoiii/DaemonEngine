#pragma once
#include "MemoryMetrics.h"

namespace Daemon
{

	class MemoryManager
	{
	public:
		static void Initialize();
		static void Shutdown();

		static MemoryMetrics Metrics;
		static MemoryManager* Get();
	private:
		static MemoryManager* s_Instance;
	};

}
