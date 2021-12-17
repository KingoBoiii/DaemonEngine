#pragma once

namespace Daemon
{

	class Allocator
	{
	public:
		static void* Allocate(size_t size);

		static void Free(void* data, size_t size);
	};

}
