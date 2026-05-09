#pragma once
#include"Core.h"
namespace Opal
{
	class OPAL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	};
	//该函数由客户端实现
	Application* CreateApplication();
}