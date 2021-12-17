#pragma once
#include <stdint.h>

namespace Daemon {

	struct MemoryMetrics {
		uint64_t TotalAllocatedMemory;	// Bytes
		uint64_t TotalFreedMemory;		// Bytes

		// Returns currently memory usage in bytes
		uint64_t GetCurrentMemoryUsage() {
			return TotalAllocatedMemory - TotalFreedMemory;
		}
	};

}