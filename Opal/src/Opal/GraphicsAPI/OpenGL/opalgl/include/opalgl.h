#pragma once
#include"Opal/Core.h"
#if defined(_WIN32) && !defined(OPAL_PLATFORM_WINDOWS)
#define OPAL_PLATFORM_WINDOWS
#endif
#define ESCAPRE 1


#define RIGHT_SHIFT 2
#define RIGHT_CTRL 4
#define RIGHT_ALT 6
#define LEFT_SHIFT 8
#define LEFT_CTRL 10
#define LEFT_ALT 12

#define MOUSE_BOUTTON_LEFT 0
#define MOUSE_BOUTTON_RIGHT 1
#define MOUSE_BUTTON_1 2

#define KEY_RELEASE 0
#define KEY_PRESS 1
#define KEY_REPEAT 2

#define MOUSE_RELEASE 0
#define MOUSE_PRESS 1
#define MOUSE_REPEAT 2

#ifdef OPAL_PLATFORM_WINDOWS
#include<Windows.h>
#include<windowsx.h>
#include<gl/GL.h>
namespace Opal
{
	typedef void (*PFNGLCLEARPROC) (GLbitfield mask);
	typedef void (*PFNGLCLEARCOLORPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	class OPAL_API OpalWindowClass
	{
	public:
		static OpalWindowClass& GetInstance();
		void glInit(const wchar_t* iconpath);
		void glInit();
	private:
		~OpalWindowClass();
	};
	class OPAL_API GLFL
	{
	public:
		static void Init(HDC hdc,HGLRC hglrc);
		static PFNGLCLEARPROC glClear;
		static PFNGLCLEARCOLORPROC glClearColor;
	};//OpenGL函数加载器
	class OPAL_API OpalWindow
	{
	public:
		HWND hwnd = nullptr;//窗口句柄
		HDC hdc = nullptr;//设备上下文
		HGLRC hglrc = nullptr;//OpenGL渲染上下文
		void* user_data = nullptr;
		//-------------回调函数的指针，对每个被创建的窗口由用户进行重定义------------
	public:
		using WindowSizeCallback = void(*)(OpalWindow* window,int width, int height);
		WindowSizeCallback OnWindowSize = nullptr;
		using WindowCloseCallback = void(*)(OpalWindow* window);
		WindowCloseCallback OnWindowClose = nullptr;
		using KeyCallback = void(*)(OpalWindow* window,int key,int scancode,int action,int mod);
		KeyCallback OnKey = nullptr;
		using MouseButtonCallback = void(*)(OpalWindow* window, int button, int action, int mods,int x,int y);
		MouseButtonCallback OnMouseButton = nullptr;
		using MouseScrollCallback = void(*)(OpalWindow* window,double Offset);
		MouseScrollCallback OnMouseScroll = nullptr;
		using MouseMoveCallback = void(*)(OpalWindow * window,double xpos,double ypos);
		MouseMoveCallback OnMouseMove = nullptr;
	public:
		bool KeyStates[256] = { false };//按键状态数组，记录每个按键的当前状态（按下或释放）用来处理是否长按
		int RepeatDelay = 500;//判断是否为长按的延迟时间，单位为毫秒
		DWORD KeyPressTime[256] = { 0 };
		//-----鼠标长按判断------
		bool MouseButtonStates[3] = { false };
		int MouseRepeatDelayMs = 500;
		DWORD MouseButtonPressTime[3] = { 0 };
	public:
		OpalWindow(int windowtype, int widthprot, int heighprot, const wchar_t* title, int width, int heigh);
		~OpalWindow();
	public:
		void SetUserPointer(void* pointer);
		void* GetUserPointer();
		void SetKeyRepeatDelay(int ms) { RepeatDelay = ms; }
		void SetMouseRepeatDelay(int ms) { MouseRepeatDelayMs = ms; };
		void SetWindowSizeCallback(WindowSizeCallback callback) { OnWindowSize = callback; }
		void SetWindowCloseCallback(WindowCloseCallback callback) { OnWindowClose = callback; }
		void SetKeyCallback(KeyCallback callback) { OnKey = callback; }
		void SetMouseButtonCallback(MouseButtonCallback callback) { OnMouseButton = callback; };
		void SetMouseScrollCallback(MouseScrollCallback callback) { OnMouseScroll = callback; };
		void SetMouseMoveCallback(MouseMoveCallback callback) { OnMouseMove = callback; };
	};
	bool CallMessage();
	void OpalSwapBuffers(OpalWindow* window);
}
#endif