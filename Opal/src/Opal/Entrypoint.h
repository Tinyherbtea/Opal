#pragma once
#ifdef OPAL_PLATFORM_WINDOWS
extern Opal::Application* Opal::CreateApplication();
int main()
{
	Opal::Log::Init();
	OPAL_CORE_WARN("This is a log");

	auto app = Opal::CreateApplication();
	app->Run();
	delete app;
}
#endif