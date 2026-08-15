#include"opalpch.h"
#include "Log.h"
namespace Opal
{
	OpalLog::Sink* Log::CoreSink = nullptr;
	OpalLog::Sink* Log::ClientSink = nullptr;
	void Log::Init()
	{
		OpalLog::pattern_set("%^[%T][%n]:%v%$");

		CoreSink = OpalLog::CreateSink(OpalLog::SinkType::Console, 4096 * 16);
		OpalLog::RegisterSink(CoreSink->GetQueue(), OpalLog::Level::trace,"Opal");

		ClientSink = OpalLog::CreateSink(OpalLog::SinkType::Console, 4096 * 16);
		OpalLog::RegisterSink(ClientSink->GetQueue(), OpalLog::Level::trace,"Stain");
	}
}