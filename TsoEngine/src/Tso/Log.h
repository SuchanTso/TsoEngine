#pragma once
#define SPDLOG_DISABLE_TID_CACHING
#include "spdlog/spdlog.h"
#include "Core.h"
#include <memory>
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Tso {
	class TSO_API Log
	{
	public:
		Log();
		~Log();


		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	};
}



// core log macros
#define TSO_CORE_TRACE(...)  ::Tso::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TSO_CORE_INFO(...)   ::Tso::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TSO_CORE_WARN(...)   ::Tso::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TSO_CORE_ERROR(...)  ::Tso::Log::GetCoreLogger()->error(__VA_ARGS__)
#define TSO_CORE_FATAL(...)  ::Tso::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// core client macros
#define TSO_TRACE(...)  ::Tso::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TSO_INFO(...)   ::Tso::Log::GetClientLogger()->info(__VA_ARGS__)
#define TSO_WARN(...)   ::Tso::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TSO_ERROR(...)  ::Tso::Log::GetClientLogger()->error(__VA_ARGS__)
#define TSO_FATAL(...)  ::Tso::Log::GetClientLogger()->fatal(__VA_ARGS__)
