#pragma once
#include"Opal/Core.h"
#if defined(_WIN32) && !defined(OPAL_PLATFORM_WINDOWS)
#define OPAL_PLATFORM_WINDOWS
#endif
#ifdef OPAL_PLATFORM_WINDOWS
#include<Windows.h>
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
		OpalWindow(int windowtype, int widthprot, int heighprot, const wchar_t* title, int width, int heigh);
		~OpalWindow();
	public:
		void SetUserPointer(void* pointer);
		void* GetUserPointer();
	public:
		HWND hwnd = nullptr;//窗口句柄
		HDC hdc = nullptr;//设备上下文
		HGLRC hglrc = nullptr;//OpenGL渲染上下文
		void (*OnWindowSize)(int width, int height, void* user_data) = nullptr;//回调函数的指针，对每个被创建的窗口由用户进行重定义
		void* WindowSizeData = nullptr;
	};
	bool CallMessage();
	void OpalSwapBuffers(OpalWindow* window);
}
#endif