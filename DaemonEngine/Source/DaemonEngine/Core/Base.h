#pragma once
#include <memory>

namespace Daemon
{

	void InitializeCore();
	void ShutdownCore();

}

#define KE_EXPAND_MACRO(x) x
#define KE_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define KE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define KINGO_STATIC_LIB

#ifdef KINGO_STATIC_LIB
	#define KINGO_API
#else
	#ifdef KINGO_EXPORT_DLL
		#define KINGO_API __declspec(dllexport)
	#else
		#define KINGO_API __declspec(dllimport)
	#endif
#endif

#include "Assert.h"

namespace Daemon
{

	template<typename T>
	using Unique = std::unique_ptr<T>;
	template<typename T, typename ...Args>
	constexpr Unique<T> CreateUnique(Args&& ...args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Shared = std::shared_ptr<T>;
	template<typename T, typename ...Args>
	constexpr Shared<T> CreateShared(Args&& ...args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}