#pragma once

#include"OpalLog.h"
#include"Core.h"


namespace Opal
{
	class OPAL_API Log
	{
	public:
		static void Init();
	private:
		static OpalLog::Sink* CoreSink;
		static OpalLog::Sink* ClientSink;
	};

}

#define OPAL_CORE_TRACE(...)   ::OpalLog::Log(::OpalLog::Level::trace, __VA_ARGS__)
#define OPAL_CORE_DEBG(...)   ::OpalLog::Log(::OpalLog::Level::debug, __VA_ARGS__)
#define OPAL_CORE_INFO(...)    ::OpalLog::Log(::OpalLog::Level::info, __VA_ARGS__)
#define OPAL_CORE_WARN(...)    ::OpalLog::Log(::OpalLog::Level::warn, __VA_ARGS__)
#define OPAL_CORE_ERROR(...)   ::OpalLog::Log(::OpalLog::Level::error, __VA_ARGS__)
#define OPAL_TRACE(...)        ::OpalLog::Log(::OpalLog::Level::trace, __VA_ARGS__)
#define OPAL_DEBUG(...)        ::OpalLog::Log(::OpalLog::Level::debug, __VA_ARGS__)
#define OPAL_INFO(...)         ::OpalLog::Log(::OpalLog::Level::info, __VA_ARGS__)
#define OPAL_WARN(...)         ::OpalLog::Log(::OpalLog::Level::warn, __VA_ARGS__)
#define OPAL_ERROR(...)        ::OpalLog::Log(::OpalLog::Level::error, __VA_ARGS__)