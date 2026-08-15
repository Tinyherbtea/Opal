workspace "Opal"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir ="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--包括相对于根文件夹（解决方案文件夹）的目录路径
--IncludeDir = {}
--IncludeDir["GLFW"] = "Opal/vendor/GLFW/include"
--include "Opal/vendor/GLFW"

project "Opal"
	location "Opal"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/"..outputdir .."/%{prj.name}")
	objdir ("bin-intermediate/"..outputdir .."/%{prj.name}")

	pchheader "opalpch.h"
	pchsource "Opal/src/opalpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	includedirs
	{
		"%{prj.name}/src",
		--"%{IncludeDir.GLFW}"
	}
	links
	{
		--"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "10.0.26100.0"

	defines
	{
		"OPALLOG_EXPORTS",
		"OPAL_PLATFORM_WINDOWS",
		"OPAL_BUILD_DLL"
	}
	
	postbuildcommands
	{
		"{MKDIR} ..\\bin\\" .. outputdir .. "\\Sandbox", 
		 ("{COPYFILE} %{cfg.targetdir}/Opal.dll ..\\bin\\" .. outputdir .. "\\Sandbox/")
	}

	filter { "system:windows", "configurations:Debug" }
        buildoptions { "/MDd" }

	filter { "system:windows", "configurations:Release" }
        buildoptions { "/MD" }

	filter { "system:windows", "configurations:Dist" }
        buildoptions { "/MD" }

	filter "configurations:Debug"
		defines "OPAL_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "OPAL_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "OPAL_DIST"
		optimize "On"

	

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	targetdir ("bin/"..outputdir .."/%{prj.name}")
	objdir ("bin-intermediate/"..outputdir .."/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	includedirs
	{
		"Opal/src"
	}

	links
	{
		"Opal"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "10.0.26100.0"

	defines
	{
		"OPAL_PLATFORM_WINDOWS"
	}

	filter { "system:windows", "configurations:Debug" }
        buildoptions { "/MDd" }

	filter { "system:windows", "configurations:Release" }
        buildoptions { "/MD" }

	filter { "system:windows", "configurations:Dist" }
        buildoptions { "/MD" }

	filter "configurations:Debug"
		defines "OPAL_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "OPAL_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "OPAL_DIST"
		optimize "On"