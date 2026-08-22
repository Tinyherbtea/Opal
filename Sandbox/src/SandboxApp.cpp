#include<Opal.h>
#include"Opal/Gui/GuiLayer.h"
#include <chrono>
class ExampleLayer :public Opal::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{

	}
	void OnAttach()override
	{
		
	}
	void OnUpdate() override
	{
		auto currentTime = std::chrono::steady_clock::now();
		auto duration = currentTime - m_LastTime;
		float deltaTime = std::chrono::duration<float>(duration).count();
		m_LastTime = currentTime; 
		float fps = 1.0f / deltaTime;
		OPAL_INFO("FPS:",fps);
	}
	void OnEvent(Opal::Event& event) override
	{
		OPAL_TRACE("{}",event);
	}
private:
	std::chrono::steady_clock::time_point m_LastTime;
};

class Stains : public Opal::Application
{
public:
	Stains()
	{
		PushLayer(new ExampleLayer());
		PushLayer(new Opal::GuiLayer());
	}
	~Stains()
	{

	}

};

Opal::Application* Opal::CreateApplication()
{
	return new Stains();
}