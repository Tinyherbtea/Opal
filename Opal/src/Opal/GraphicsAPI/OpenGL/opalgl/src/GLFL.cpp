#include"opalpch.h"
#include"../include/opalgl.h"
namespace Opal
{
	PFNGLCLEARPROC GLFL::glClear = nullptr;
	PFNGLCLEARCOLORPROC GLFL::glClearColor = nullptr;
	void GLFL::Init(HDC hdc,HGLRC hglrc)
	{
		glClear = (PFNGLCLEARPROC)wglGetProcAddress("glClear");
		glClearColor = (PFNGLCLEARCOLORPROC)wglGetProcAddress("glClearColor");
	}
	
}
