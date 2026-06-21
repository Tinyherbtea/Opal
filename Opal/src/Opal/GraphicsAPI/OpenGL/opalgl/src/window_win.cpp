#include"opalpch.h"
#include"../include/opalgl.h"

#define WGL_CONTEXT_MAJOR_VERSION_ARB   0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB   0x2092
#define WGL_CONTEXT_FLAGS_ARB           0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB    0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int* attribList);//定义函数指针类型，用于加载wglCreateContextAttribsARB函数
namespace Opal
{
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        OpalWindow* window = nullptr;//获取与窗口相关联的OpalWindow结构指针
        if (msg == WM_NCCREATE)
        {
            CREATESTRUCT* cs = (CREATESTRUCT*)lParam;//WM_NCCREATE消息在窗口创建过程中发送，此时lParam指向一个CREATESTRUCT结构，包含了创建窗口时传递的参数
            window = (OpalWindow*)cs->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
        }
        else
        {
            window = (OpalWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        switch (msg)
        {

        case WM_DESTROY:
            PostQuitMessage(0);//发送WM_QUIT消息，此消息导致 GetMessage 函数返回零。
            return 0;
        case WM_SIZE:
            if (window && window->OnWindowSize)
                window->OnWindowSize(LOWORD(lParam), HIWORD(lParam), window->WindowSizeData);
            break;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    OpalWindowClass& OpalWindowClass::GetInstance()
    {
        static OpalWindowClass instance;
        return instance;
    }
    void OpalWindowClass::glInit(const wchar_t* iconpath)//创建窗口类模板
    {
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(wc);
        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;        // 设置消息处理函数
        wc.hInstance = GetModuleHandle(NULL);  // 获取当前实例句柄
        HICON hIcon = (HICON)LoadImage(
            wc.hInstance,                          // 程序实例句柄
             iconpath,            // 图标文件的路径
            IMAGE_ICON,                         // 指定加载的是图标
            32, 32,                             // 期望的宽高（32x32）
            LR_LOADFROMFILE | LR_DEFAULTSIZE);    // 从文件加载，并使用默认尺寸
        if (hIcon)
        {
            wc.hIcon = hIcon;                    // 设置窗口图标
            wc.hIconSm = hIcon;                  // 设置小图标（任务栏图标）
        }
        else
        {
            wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
            wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        }
        wc.lpszClassName = L"OpalWindowClass";    // 自定义窗口类名（避免冲突）
        wc.hCursor = NULL;  // 设置鼠标指针
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        if (!RegisterClassEx(&wc))
        {
            MessageBox(NULL, L"Failed to register window class", L"Error", MB_ICONERROR);
            return;
        }
    };
        void OpalWindowClass::glInit()//创建窗口类模板
        {
            WNDCLASSEX wc = {};
            wc.cbSize = sizeof(wc);
            wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
            wc.lpfnWndProc = WndProc;        // 设置消息处理函数
            wc.hInstance = GetModuleHandle(NULL);  // 获取当前实例句柄
            wc.lpszClassName = L"OpalWindowClass";    // 自定义窗口类名（避免冲突）
            wc.hCursor = NULL;  // 设置鼠标指针
            wc.hbrBackground = NULL;
            wc.lpszMenuName = NULL;
            if (!RegisterClassEx(&wc))
            {
                MessageBox(NULL, L"Failed to register window class", L"Error", MB_ICONERROR);
                return;
            }
        };

        OpalWindowClass::~OpalWindowClass()
        {
            UnregisterClass(L"OpalWindowClass", GetModuleHandle(NULL));
        }


        OpalWindow::OpalWindow(int windowtype, int widthprot, int heighprot, const wchar_t* title, int width, int heigh)//按一个窗口类模板创建窗口并返回指向OpalWindow结构的指针
        {
            PIXELFORMATDESCRIPTOR pfd = {
           sizeof(PIXELFORMATDESCRIPTOR),    // 这个pfd的大小  
           1,                                // 版本号，保留，必须为1
           PFD_DRAW_TO_WINDOW |              // 支持窗口
           PFD_SUPPORT_OPENGL |              // 支持OpenGL  
           PFD_DOUBLEBUFFER,                 // 双缓冲  
           PFD_TYPE_RGBA,                    // RGBA类型  
           24,                               // 24位颜色深度  
           0, 0, 0, 0, 0, 0,                 // 忽略颜色位  
           0,                                // 无alpha缓冲  
           0,                                // 移位位数
           0,                                // 无累加缓冲
           0, 0, 0, 0,                       // 累加位忽略  
           32,                               // 32位深度缓冲      
           0,                                // 无模板缓冲  
           0,                                // 无辅助缓冲  
           PFD_MAIN_PLANE,                   // 主层  
           0,                                // 是否保留
           0, 0, 0                           // 层遮罩忽略 
            };
            hwnd = CreateWindowEx(
                0,                                      //正在创建的窗口的扩展窗口样式。(0即无扩展)
                L"OpalWindowClass",                     // 窗口类模板名（比如L"OpalWindowClass"会使用名为L"OpalWindowClass"的类模板）
                title,                                  // 窗口文本
                windowtype,                             // 窗口样式：WS_OVERLAPPEDWINDOW | WS_VISIBLE带标题栏/边框/最大化按钮 + 初始可见
                widthprot, heighprot,           // 初始位置,以左上角为原点，依次为width与high
                width, heigh,                           // 初始的宽高
                NULL,                                   // 正在创建的窗口的父窗口或所有者窗口的句柄。
                NULL,                                   // 菜单句柄
                GetModuleHandle(NULL),                  // 要与窗口关联的模块实例的句柄。
                this                                    // 附加参数
            );
            hdc = GetDC(hwnd);

            int iPixelFormat;
            iPixelFormat = ChoosePixelFormat(hdc, &pfd);
            SetPixelFormat(hdc, iPixelFormat, &pfd);

            HGLRC temhglrc = wglCreateContext(hdc);
            wglMakeCurrent(hdc, temhglrc);
            PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(temhglrc);
            int attribs[] = {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                WGL_CONTEXT_MINOR_VERSION_ARB, 6,
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,//WGL_CONTEXT_PROFILE_MASK_ARB:现代 OpenGL 模式，彻底移除了所有过时的固定管线功能
                //WGL_CONTEXT_CORE_PROFILE_BIT_ARB:请求一个调试上下文，驱动会提供更详细的错误输出和着色器调试信息。
                0 // 必须以 0 结尾
            };
            hglrc = wglCreateContextAttribsARB(hdc, 0, attribs);
            wglMakeCurrent(hdc, hglrc);
            GLFL::Init(hdc,hglrc);
        }
        OpalWindow::~OpalWindow()
        {
            if (hglrc) {
                wglMakeCurrent(nullptr, nullptr);
                wglDeleteContext(hglrc);
            }
            if (hdc && hwnd) {
                ReleaseDC(hwnd, hdc);
            }
            if (hwnd) {
                DestroyWindow(hwnd);
            }
        }

        void OpalWindow::SetUserPointer(void* pointer)
        {
            // 将指针存储在 GWLP_USERDATA 中
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pointer);
        }
        void* OpalWindow::GetUserPointer()
        {
            // 从 GWLP_USERDATA 中取回指针
            return (void*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        bool CallMessage()
        {
            MSG msg;
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            if (msg.message==WM_QUIT)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        void OpalSwapBuffers(OpalWindow* window)
        {
            SwapBuffers(window->hdc);
        }
}
