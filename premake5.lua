workspace "TsoEngine"
	-- architecture "x64"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "TsoEngine/third_party/GLFW/include"
IncludeDir["Glad"] = "TsoEngine/third_party/Glad/include"
IncludeDir["imgui"] = "TsoEngine/third_party/imgui"

include "TsoEngine/third_party/GLFW"
include "TsoEngine/third_party/Glad"
include "TsoEngine/third_party/imgui"

project "TsoEngine"
	location "TsoEngine"
	kind "SharedLib"
	language "C++"
	targetdir ("bin/" .. outputdir .."/%{prj.name}")
	objdir ("bin-int/" .. outputdir .."/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"%{prj.name}/third_party/spdlog/include",
		"%{prj.name}/src",
		"%{prj.name}/third_party/GLFW/include",
		"%{prj.name}/third_party/Glad/include",
		"%{prj.name}/third_party/imgui"
	}

	links{
		"GLFW",
		"Glad",
		"imgui"
		--"opengl32.lib"
	}


	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		pchheader "TPch.h"
		pchsource "%{prj.name}/src/TPch.cpp"

		defines{
			"TSO_PLATFORM_WINDOWS",
			"TSO_BUILD_DLL"
		}

		

		postbuildcommands{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/sandbox")
		}

	filter "system:macosx"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines{
			"TSO_PLATFORM_MACOSX",
			"TSO_BUILD_DYLIB"
		}
		

	filter "configurations:Debug"
		defines "TSO_DEBUG"
		-- buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "TSO_RELEASE"
		-- buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "TSO_DIST"
		-- buildoptions "/MD"
		optimize "On"


project "Sandbox"
	location "sandbox"
	kind "ConsoleApp"
	language "C++"
	targetdir ("bin/" .. outputdir .."/%{prj.name}")
	objdir ("bin-int/" .. outputdir .."/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"TsoEngine/third_party/spdlog/include",
		"TsoEngine/src"
	}

	links{
		"TsoEngine"
	}

	filter "system:windows"
		defines{
				"TSO_PLATFORM_WINDOWS"
		}
	
	filter "system:macosx"
		defines{
				"TSO_PLATFORM_MACOSX"
		}

	filter "configurations:Debug"
		defines "TSO_DEBUG"
		--buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "TSO_RELEASE"
		-- buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "TSO_DIST"
		-- buildoptions "/MD"
		optimize "On"