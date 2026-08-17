#include"opalpch.h"
#include"../include/opalgl.h"
namespace Opal
{
	PFNGLGENVERTEXARRAYSPROC GLFL::glGenVertexArrays = nullptr;
    PFNGLCREATEPROGRAMPROC GLFL::glCreateProgram = nullptr;
    PFNGLCREATESHADERPROC GLFL::glCreateShader = nullptr;
    PFNGLSHADERSOURCEPROC GLFL::glShaderSource = nullptr;
    PFNGLCOMPILESHADERPROC GLFL::glCompileShader = nullptr;
    PFNGLATTACHSHADERPROC GLFL::glAttachShader = nullptr;
    PFNGLLINKPROGRAMPROC GLFL::glLinkProgram = nullptr;
    PFNGLUSEPROGRAMPROC GLFL::glUseProgram = nullptr;
    PFNGLDELETEPROGRAMPROC GLFL::glDeleteProgram = nullptr;
    PFNGLDELETESHADERPROC GLFL::glDeleteShader = nullptr;
    PFNGLGETUNIFORMLOCATIONPROC GLFL::glGetUniformLocation = nullptr;
    PFNGLUNIFORM4FPROC GLFL::glUniform4f = nullptr;
    PFNGLGENBUFFERSPROC GLFL::glGenBuffers = nullptr;
    PFNGLBINDBUFFERPROC GLFL::glBindBuffer = nullptr;
    PFNGLBUFFERDATAPROC GLFL::glBufferData = nullptr;
    PFNGLBUFFERSUBDATAPROC GLFL::glBufferSubData = nullptr;
    PFNGLDELETEBUFFERSPROC GLFL::glDeleteBuffers = nullptr;
    PFNGLENABLEVERTEXATTRIBARRAYPROC GLFL::glEnableVertexAttribArray = nullptr;
    PFNGLVERTEXATTRIBPOINTERPROC GLFL::glVertexAttribPointer = nullptr;
    PFNGLBINDVERTEXARRAYPROC GLFL::glBindVertexArray = nullptr;
    PFNGLDELETEVERTEXARRAYSPROC GLFL::glDeleteVertexArrays = nullptr;
    PFNGLGETSHADERIVPROC GLFL::glGetShaderiv = nullptr;
    PFNGLGETSHADERINFOLOGPROC GLFL::glGetShaderInfoLog = nullptr;
	void GLFL::Init(HDC hdc,HGLRC hglrc)
	{
		glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
        glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
        glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
        glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
        glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
        glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
        glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
        glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
        glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
        glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
        glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
        glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
        glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
        glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
        glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
        glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
        glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
        glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
        glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
        glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
		glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
		glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	}
	
}
