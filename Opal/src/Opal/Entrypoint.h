#pragma once
#ifdef OPAL_PLATFORM_WINDOWS
extern Opal::Application* Opal::CreateApplication();
int main()
{

	auto app = Opal::CreateApplication();
	app->Run();
	delete app;
}
#endif