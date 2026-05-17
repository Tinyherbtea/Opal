#pragma once

#include"OpalLog.h"
#include"Core.h"


namespace Opal
{
	class OPAL_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<OpalLog::Logger>& GetCoreLogger() { return CoreLogger; }
		inline static std::shared_ptr<OpalLog::Logger>& GetClientLogger() { return ClientLogger; }
	private:
		static std::shared_ptr<OpalLog::Logger> CoreLogger;
		static std::shared_ptr<OpalLog::Logger> ClientLogger;
	};

}

#define OPAL_CORE_TRACE(...)   ::Opal::Log::GetCoreLogger()->Trace(__VA_ARGS__)
#define OPAL_CORE_FATAL(...)   ::Opal::Log::GetCoreLogger()->Debug(__VA_ARGS__)
#define OPAL_CORE_INFO(...)    ::Opal::Log::GetCoreLogger()->Info(__VA_ARGS__)
#define OPAL_CORE_WARN(...)    ::Opal::Log::GetCoreLogger()->Warn(__VA_ARGS__)
#define OPAL_CORE_ERROR(...)   ::Opal::Log::GetCoreLogger()->Error(__VA_ARGS__)

#define OPAL_TRACE(...)        ::Opal::Log::GetClientLogger()->Trace(__VA_ARGS__)
#define OPAL_FATAL(...)        ::Opal::Log::GetClientLogger()->Debug(__VA_ARGS__)
#define OPAL_INFO(...)         ::Opal::Log::GetClientLogger()->Info(__VA_ARGS__)
#define OPAL_WARN(...)         ::Opal::Log::GetClientLogger()->Warn(__VA_ARGS__)
#define OPAL_ERROR(...)        ::Opal::Log::GetClientLogger()->Error(__VA_ARGS__)