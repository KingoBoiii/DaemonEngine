#include "kepch.h"
#include "MemoryManager.h"

namespace Daemon
{

    MemoryManager* MemoryManager::s_Instance = nullptr;
    MemoryMetrics MemoryManager::Metrics = MemoryMetrics();

    void MemoryManager::Initialize()
    {
    }

    void MemoryManager::Shutdown()
    {
        memset(&Metrics, 0, sizeof(MemoryMetrics));
        delete s_Instance;
    }

    MemoryManager* MemoryManager::Get()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = (MemoryManager*)malloc(sizeof(MemoryManager));
            s_Instance = new(s_Instance) MemoryManager();
        }
        return s_Instance;
    }

}