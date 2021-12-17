#pragma once
#include "Allocator.h"

#pragma warning(disable : 4595)

inline void* operator new(size_t size)
{
	return Daemon::Allocator::Allocate(size);
}

inline void* operator new[](size_t size)
{
	return Daemon::Allocator::Allocate(size);
}

inline void operator delete(void* block, size_t size)
{
	return Daemon::Allocator::Free(block, size);
}

inline void operator delete[](void* block, size_t size)
{
	return Daemon::Allocator::Free(block, size);
}

#pragma warning(default : 4595)
