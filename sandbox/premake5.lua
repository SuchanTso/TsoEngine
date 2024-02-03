project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir  ("../bin/"..outputdir.."/%{prj.name}")
	objdir  ("../bin-int/"..outputdir.."/%{prj.name}")
    
	files { "src/**.h", "src/**.cpp"}

    includedirs
	{
        "%{wks.location}/TsoEngine/third_party/spdlog/include",
		"%{wks.location}/TsoEngine/src",
		"%{wks.location}/TsoEngine/src/Tso",
		"%{wks.location}/TsoEngine/third_party/glm",
		"%{wks.location}/TsoEngine/third_party/imgui",
		"%{wks.location}/TsoEngine/third_party/entt",
		"%{wks.location}/TsoEngine/third_party/yaml-cpp/include",
		"%{wks.location}/TsoEngine/third_party/box2d/include",
		"%{wks.location}/TsoEngine/third_party/mono/include"


		--"Hazel/vendor/box2D/include"
	}

	links { "TsoEngine" }

    filter  "system:windows" 
	    systemversion "latest"
		defines { "TSO_PLATFORM_WINDOWS"}

		


	filter "system:macosx"
		 defines{"TSO_PLATFORM_MACOSX"}
		 links{
			 "Cocoa.framework",
			 "IOKit.framework",
			 "CoreVideo.framework",
			 "OpenGL.framework"
		 }

	filter { "configurations:Debug"}
		 defines { "DEBUG"}
		 symbols "On"
		 if _ACTION == "vs2022" then
			 buildoptions "/MTd"
		 end

	filter { "configurations:Release"}
		 defines { "NDEBUG" }
		 optimize "On"
		 if _ACTION == "vs2022" then
			 buildoptions "/MT"
		 end

	filter { "configurations:Dist"}
		 defines { "NDEBUG"}
		 optimize "On"
		 if _ACTION == "vs2022" then
			 buildoptions "/MT"
		 end