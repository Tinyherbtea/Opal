#include "opalpch.h"
#include "Opal/Platform/Windows/WindowsWindow.h"

#include"Opal/Log.h"

#include "Opal/Events/KeyEvent.h"
#include "Opal/Events/ApplicationEvent.h"
#include "Opal/Events/MouseEvent.h"

namespace Opal
{
	static bool s_OpalGLInitialized = false;

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		OPAL_CORE_INFO("Creating window {} ({}, {})", props.Title.c_str(), props.Width, props.Height);

		if (!s_OpalGLInitialized)
		{
			OpalWindowClass::GetInstance().glInit();
			OPAL_CORE_ASSERT(success,"Could not Initialize GLFW!");
			s_OpalGLInitialized = true;
		}
		std::wstring wTitle(m_Data.Title.begin(), m_Data.Title.end());
		m_Window = new OpalWindow(WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0,wTitle.c_str(), (int)props.Width, (int)props.Height);
		m_Window->SetUserPointer(&m_Data);
		SetVSync(true);
		m_Window->SetWindowSizeCallback([](OpalWindow* window,int width, int height) 
			{
				WindowData& data = *(WindowData*)window->GetUserPointer();
				WindowResizeEvent event(width,height);
				data.EventCallback(event);
				data.Width = width;
				data.Height = height;
			});
		m_Window->SetWindowCloseCallback([](OpalWindow* window)
			{
				WindowData& data = *(WindowData*)window->GetUserPointer();
				WindowCloseEvent event;
				data.EventCallback(event);
			});
		m_Window->SetKeyCallback([](OpalWindow* window, int key, int scancode, int action, int mod)
			{
				WindowData& data = *(WindowData*)window->GetUserPointer();
				
				switch (action)
				{
				case KEY_PRESS :
				{
					KeyPressedEvent event(key,0);
					data.EventCallback(event);
					break;
				}
				case KEY_RELEASE :
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case KEY_REPEAT :
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
				}
			});
		m_Window->SetMouseButtonCallback([](OpalWindow* window, int button, int action, int mods, int x, int y)
			{
				WindowData& data = *(WindowData*)window->GetUserPointer();
				switch (action)
				{
				case MOUSE_PRESS :
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case MOUSE_REPEAT :
				{

					break;
				}
				case MOUSE_RELEASE :
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
				}
			});
		m_Window->SetMouseScrollCallback([](OpalWindow* window,double Offset) 
			{
				WindowData& data = *(WindowData*)window->GetUserPointer();

				MouseScrolledEvent event((float)Offset);
				data.EventCallback(event);
			});
		m_Window->SetMouseMoveCallback([](OpalWindow* window,double xpos,double ypos)
			{
				WindowData& data = *(WindowData*)window->GetUserPointer();

				MouseMovedEvent event((float)xpos,(float)ypos);
				data.EventCallback(event);
			});
	}
	void WindowsWindow::Shutdown()
	{
		if (m_Window) 
		{
			delete m_Window;
			m_Window = nullptr;
		}//窗口资源由OpalWindow类的析构函数负责释放
	}
	void WindowsWindow::OnUpdate()
	{
		CallMessage();
		OpalSwapBuffers(m_Window);

	}
	void WindowsWindow::SetVSync(bool enable)
	{
		/*
		if (enable)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enable;
		*/
	}
	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}