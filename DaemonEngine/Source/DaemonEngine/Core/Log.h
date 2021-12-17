#pragma once

#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Daemon
{

	class Log
	{
	public:
		static void Init();
		static void Shutdown();

		static std::shared_ptr<spdlog::logger> GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger> GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define KE_CORE_TRACE(...)    ::Daemon::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define KE_CORE_INFO(...)     ::Daemon::Log::GetCoreLogger()->info(__VA_ARGS__)
#define KE_CORE_WARN(...)     ::Daemon::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define KE_CORE_ERROR(...)    ::Daemon::Log::GetCoreLogger()->error(__VA_ARGS__)
#define KE_CORE_CRITICAL(...) ::Daemon::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define KE_TRACE(...)         ::Daemon::Log::GetClientLogger()->trace(__VA_ARGS__)
#define KE_INFO(...)          ::Daemon::Log::GetClientLogger()->info(__VA_ARGS__)
#define KE_WARN(...)          ::Daemon::Log::GetClientLogger()->warn(__VA_ARGS__)
#define KE_ERROR(...)         ::Daemon::Log::GetClientLogger()->error(__VA_ARGS__)
#define KE_CRITICAL(...)      ::Daemon::Log::GetClientLogger()->critical(__VA_ARGS__)