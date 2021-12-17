#include "kepch.h"
#include "Allocator.h"
#include "MemoryManager.h"

namespace Daemon
{

    void* Allocator::Allocate(size_t size)
    {
        Daemon::MemoryManager::Metrics.TotalAllocatedMemory += size;
        return malloc(size);
    }

    void Allocator::Free(void* data, size_t size)
    {
        Daemon::MemoryManager::Metrics.TotalFreedMemory += size;
        free(data);
    }

}