#pragma once

#if defined(_WIN32)
	#if defined(_WIN64)
		#define KE_PLATFORM_WINDOWS
	#else
		#error DaemonEngine doesn't support x86 (32 bit) builds
	#endif
#elif defined(__linux__)
	#define KE_PLATFORM_LINUX
#elif defined(__APPLE__) || defined(__MACH__)
	#define KE_PLATFORM_IOS
#else
	#error Unknown platform
#endif

