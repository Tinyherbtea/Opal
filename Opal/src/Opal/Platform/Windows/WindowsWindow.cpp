#include "opalpch.h"
#include "Opal/Platform/Windows/WindowsWindow.h"
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