#pragma once
#include"Core.h"

#include"Opal/GraphicsAPI/OpenGL/opalgl/include/opalgl.h"

#include"Window.h"
#include"LayerStack.h"
#include"Events/Event.h"
#include"Events/ApplicationEvent.h"


namespace Opal
{
	class OPAL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};
	//该函数由客户端实现
	Application* CreateApplication();
}