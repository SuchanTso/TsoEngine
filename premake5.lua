workspace "TsoEngine"
	architecture "x64"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
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
		"%{prj.name}/src"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines{
			"TSO_PLATFORM_WINDOWS",
			"TSO_BUILD_DLL"
		}

		postbuildcommands{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/sandbox")
		}

		filter "configurations:Debug"
			defines "TSO_DEBUG"
			symbols "On"

		filter "configurations:Release"
			defines "TSO_RELEASE"
			optimize "On"

		filter "configurations:Dist"
			defines "TSO_DIST"
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

	defines{
			"TSO_PLATFORM_WINDOWS"
	}

		filter "configurations:Debug"
			defines "TSO_DEBUG"
			symbols "On"

		filter "configurations:Release"
			defines "TSO_RELEASE"
			optimize "On"

		filter "configurations:Dist"
			defines "TSO_DIST"
			optimize "On"