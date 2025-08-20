project "TsoEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir  ("../bin/"..outputdir.."/%{prj.name}")
	objdir  ("../bin-int/"..outputdir.."/%{prj.name}")
    
	files { "src/**.h", "src/**.cpp"}

	defines
	{
		"YAML_CPP_STATIC_DEFINE",
		"TSO_EDITOR"
	}

    externalincludedirs
	{
        "%{wks.location}/TsoEngine/third_party/spdlog/include",
		"%{wks.location}/TsoEngine/src",
		"%{wks.location}/TsoEngine/src/Tso",
		"%{wks.location}/TsoEngine/third_party/glm",
		"%{wks.location}/TsoEngine/third_party/imgui",
		"%{wks.location}/TsoEngine/third_party/entt",
		"%{wks.location}/TsoEngine/third_party/yaml-cpp/include",
		"%{wks.location}/TsoEngine/third_party/box2d/include",
		"%{wks.location}/TsoEngine/third_party/mono/include",
		"%{wks.location}/TsoEngine/third_party/msdf-atlas-gen/msfgen/include",
		"%{wks.location}/TsoEngine/third_party/msdf-atlas-gen/msdf-atlas-gen",
		"%{wks.location}/TsoEngine/third_party/msdf-atlas-gen/msdfgen",
		"%{wks.location}/TsoEngine/third_party/readerwriterqueue",
		"%{wks.location}/TsoEngine/third_party/lua/src",
		"%{wks.location}/TsoEngine/third_party/sol2/include",
		"%{wks.location}/TsoEngine/third_party/utfcpp",

	}

	links { "TsoEngine" }

	filter  "system:windows"
		postbuildcommands
		{
			"xcopy \"%{wks.location}/TsoEditor/assets\" \"%{cfg.targetdir}/assets\" /S /E /Y /I",
			"{COPY} %{wks.location}/TsoEditor/imgui.ini %{cfg.targetdir}/assets"
		}

	filter  "system:macosx"
		postbuildcommands
		{
			"cp -ru \"%{wks.location}/TsoEditor/assets/.\" \"%{cfg.targetdir}/assets/\"",
			"{COPY} %{wks.location}/TsoEditor/imgui.ini %{cfg.targetdir}/assets"
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

	

    filter  "system:windows" 
	    systemversion "latest"
		 defines { "TSO_PLATFORM_WINDOWS"}
		 			
		links
		{
		    -- windows needed libs for mono
			"Ws2_32.lib",
			"Bcrypt.lib",
			"Version.lib",
			"Winmm.lib"
		}

		

	filter "system:macosx"
	kind "WindowedApp"
		defines{
				"TSO_PLATFORM_MACOSX"

		}
		links{
			"Cocoa.framework",
			"IOKit.framework",
			"CoreVideo.framework",
			"OpenGL.framework"
		}
		xcodebuildsettings 
		{
			GENERATE_INFOPLIST_FILE = "YES",
			-- 建议添加一个产品包标识符
			-- 格式通常是反向域名：com.yourcompany.yourapp
			PRODUCT_BUNDLE_IDENTIFIER = "com.mycompany.myawesomeapp",
			--  MACOSX_DEPLOYMENT_TARGET = "11.0" -- (可选) 设置最低支持的 macOS 版本
		}
		filter "files:asset/**"
			print("%{prj.name}")
            buildaction "Embed"

	
