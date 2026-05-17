#pragma once
//这个文件包含了一些核心的宏定义，用于处理跨平台的动态链接库导出和导入
#ifdef OPAL_PLATFORM_WINDOWS
#ifdef OPAL_BUILD_DLL
#define OPAL_API __declspec(dllexport)
#else
#define OPAL_API __declspec(dllimport)
#endif
#else
#error Engine only support Windows!
#endif

#define BIT(x)(1<<x)