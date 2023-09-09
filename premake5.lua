workspace "TsoEngine"
	-- filter "system:windows"
	-- 	architecture "x64"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}

	startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "TsoEngine/third_party/GLFW/include"
IncludeDir["Glad"] = "TsoEngine/third_party/Glad/include"
IncludeDir["imgui"] = "TsoEngine/third_party/imgui"
IncludeDir["glm"] = "TsoEngine/third_party/glm"
IncludeDir["stb_image"] = "TsoEngine/third_party/stb_image"
IncludeDir["rapidjson"] = "TsoEngine/third_party/rapidjson"


include "TsoEngine/third_party/GLFW"
include "TsoEngine/third_party/Glad"
include "TsoEngine/third_party/imgui"




project "TsoEngine"
	location "TsoEngine"
	kind "StaticLib"
	language "C++"
	targetdir ("bin/" .. outputdir .."/%{prj.name}")
	objdir ("bin-int/" .. outputdir .."/%{prj.name}")
	staticruntime "on"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/third_party/glm/glm/**.hpp",
		"%{prj.name}/third_party/glm/glm/**.inl",
		"%{prj.name}/third_party/stb_image/**.h",
		"%{prj.name}/third_party/stb_image/**.cpp",
		"%{prj.name}/third_party/rapidjson/include/rapidjson/**.h"


	}

	links{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}


	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		pchheader "TPch.h"
		pchsource "%{prj.name}/src/TPch.cpp"

		defines{
			"TSO_PLATFORM_WINDOWS",
		}

		includedirs{
			"%{prj.name}/third_party/spdlog/include",
			"%{prj.name}/src",
			"%{prj.name}/third_party/GLFW/include",
			"%{prj.name}/third_party/Glad/include",
			"%{prj.name}/third_party/imgui",
			"%{prj.name}/third_party/glm",
			"%{prj.name}/third_party/stb_image",
			"%{prj.name}/third_party/rapidjson"
	
	
		}


	filter "system:macosx"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		pchheader "src/TPch.h"
		pchsource "%{prj.name}/src/TPch.cpp"

		defines{
			"TSO_PLATFORM_MACOSX",
		}

		sysincludedirs{
			"%{prj.name}/third_party/spdlog/include",
			"%{prj.name}/src",
			"%{prj.name}/third_party/GLFW/include",
			"%{prj.name}/third_party/Glad/include",
			"%{prj.name}/third_party/imgui",
			"%{prj.name}/third_party/glm",
			"%{prj.name}/third_party/stb_image",
			"%{prj.name}/third_party/rapidjson"


		}
		

	filter "configurations:Debug"
		defines "TSO_DEBUG"
		filter "system:windows"
			buildoptions "/MDd"

		symbols "On"

	filter "configurations:Release"
		defines "TSO_RELEASE"
		filter "system:windows"
			buildoptions "/MD"

		optimize "On"

	filter "configurations:Dist"
		defines "TSO_DIST"
		filter "system:windows"
			buildoptions "/MD"

		optimize "On"


project "Sandbox"
	location "sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .."/%{prj.name}")
	objdir ("bin-int/" .. outputdir .."/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"TsoEngine/third_party/spdlog/include",
		"TsoEngine/src",
		"%{IncludeDir.glm}",
		"TsoEngine/third_party/imgui",
		"TsoEngine/third_party/rapidjson/include"
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
		links{
			"Cocoa.framework",
			"IOKit.framework",
			"CoreVideo.framework",
			"OpenGL.framework"
		}

	filter "configurations:Debug"
		defines "TSO_DEBUG"

		filter "system:windows"
			buildoptions "/MDd"

		symbols "On"

	filter "configurations:Release"
		defines "TSO_RELEASE"
		filter "system:windows"
			buildoptions "/MD"

		optimize "On"

	filter "configurations:Dist"
		defines "TSO_DIST"
		filter "system:windows"
			buildoptions "/MD"

		optimize "On"