project "TsoEngine"
    kind "StaticLib"
    language "C++"
	staticruntime "off"
	cppdialect "C++17"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}") 
	objdir   ("../bin-int/" .. outputdir .. "/%{prj.name}") 

	links {"GLFW", "opengl32.lib", "Glad", "imgui", "YAML_CPP" , "Box2D" ,
	"third_party/mono/lib/Debug/mono-2.0-sgen.lib","msdf-atlas-gen","Lua"

	}
	
    pchheader "TPch.h"
    pchsource "src/TPch.cpp"

	defines
	{
	    "_CRT_SECURE_NO_WARNINGS", 
		"YAML_CPP_STATIC_DEFINE",
		"SOL_ALL_SAFETIES_ON"
	}
	
	files
	{
		"src/**.h",
		"src/**.cpp",
		"third_party/**.hpp",
		"third_party/entt/entt.hpp",
		"third_party/stb_image/**.h",
		"third_party/stb_image/**.cpp",
		"third_party/yaml-cpp/include/yaml-cpp/**.h",
		"third_party/mono/include/**.h",
		"third_party/readerwriterqueue/readerwriterqueue.h",
		"third_party/readerwriterqueue/atomicops.h",
		"third_party/sol2/include/sol/**.hpp",
		"third_party/utfcpp/**.h",
	}

	externalincludedirs
	{
		"third_party/spdlog/include",
		"src",
		"src/Tso",
		"third_party/GLFW/include",
		"third_party/Glad/include",
		"third_party/imgui",
		"third_party/glm",
		"third_party/stb_image",
		"third_party/entt",
		"third_party/yaml-cpp/include",
		"third_party/box2d/include",
		"third_party/mono/include",
		"third_party/msdf-atlas-gen/msdfgen/include",
		"third_party/msdf-atlas-gen/msdf-atlas-gen",
		"third_party/msdf-atlas-gen/msdfgen",
		"third_party/readerwriterqueue",
        "third_party/lua/src",
		"third_party/sol2/include",
		"third_party/utfcpp"

	}


	filter { "configurations:Debug" }
			defines { "TSO_DEBUG"}
			symbols "On"
			runtime "Debug" -- ����ʱ���ӵ�dll��debug���͵�	
			if _ACTION == "vs2022" then
				buildoptions "/MTd"
			end
			-- in VS2019 that is Additional Library Directories
			
			
			--links
			--{
				--"spirv-cross-cored.lib",
				--"spirv-cross-glsld.lib",
				--"SPIRV-Toolsd.lib",
				--"libmono-static-sgen.lib"
			--}
		

		filter { "configurations:Release"}
			defines { "TSO_RELEASE"}
			optimize "On"
			runtime "Release" -- ����ʱ���ӵ�dll��release���͵�
		if _ACTION == "vs2022" then
			buildoptions "/MT"
		end
		-- in VS2019 that is Additional Library Directories
		
		
		


		filter { "configurations:Dist"}
			defines { "TSO_DIST"}
			optimize "On"
		if _ACTION == "vs2022" then
			buildoptions "/MT"
		end

	filter  "system:windows" 
	    systemversion "latest"
		defines {"TSO_PLATFORM_WINDOWS", "GLFW_INCLUDE_NONE", "TSO_ENABLE_ASSERTS" , "CPPHTTPLIB_OPENSSL_SUPPORT"}
		
		postbuildcommands
		{
		    -- "copy default.config bin\\project.config"
			-- copy from relative path to ... ע�������COPYǰ��û��%
		    ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" ..outputdir.."/Sandbox/\"")
		}
		links
		{
		    -- windows needed libs for mono
			"Ws2_32.lib",
			"Bcrypt.lib",
			"Version.lib",
			"Winmm.lib"
		}



	filter "system:macosx"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		pchheader "src/TPch.h"
		pchsource "src/TPch.cpp"

		defines{
			"TSO_PLATFORM_MACOSX",
			"MACOS_BUNDLE",
			"TSO_ENABLE_ASSERTS"
		}
		files{
			"src/**.mm"
		}

		







