#include<Opal.h>

class ExampleLayer :public Opal::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{

	}
	void OnUpdate() override
	{
		OPAL_INFO("ExampleLayer::Updata");
	}
	void OnEvent(Opal::Event& event) override
	{
		OPAL_TRACE("{}",event);
	}
};

class Stains : public Opal::Application
{
public:
	Stains()
	{
		PushLayer(new ExampleLayer());
	}
	~Stains()
	{

	}

};

Opal::Application* Opal::CreateApplication()
{
	return new Stains();
}