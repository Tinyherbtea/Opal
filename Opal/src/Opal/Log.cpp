#include"opalpch.h"
#include "Log.h"
namespace Opal
{
	std::shared_ptr<OpalLog::Logger> Log::CoreLogger;
	std::shared_ptr<OpalLog::Logger> Log::ClientLogger;
	void Log::Init()
	{
		OpalLog::pattern_set("%^[%T][%n]:%v%$");

		CoreLogger = OpalLog::LogRegister::Console_("Opal");
		CoreLogger->Logger_level = OpalLog::Level::trace;
		CoreLogger->StartLoging();

		ClientLogger = OpalLog::LogRegister::Console_("Stains");
		ClientLogger->Logger_level = OpalLog::Level::trace;
		ClientLogger->StartLoging();
	}
}