-- premake5.lua
-- ----------------------------------------------------
-- 定义 Lua 静态库项目
-- ----------------------------------------------------
project "Lua"
    kind "StaticLib"      -- 关键：这是一个静态库 (.a / .lib)
    language "C"          -- 关键：Lua 是 C 代码，用 C 编译器编译

    -- 包含 Lua 的所有源文件和头文件
    files { "src/**.c", "src/**.h" }
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- 排除包含 main() 函数的文件，因为我们是库，不是可执行程序
    removefiles { "src/lua.c", "src/luac.c" }
    
    -- 为使用这个库的项目，暴露头文件目录
    -- includedirs { "vendor/lua/src" }

    -- 在 Windows 上编译 Lua 需要这个宏定义来避免一些警告
    filter "system:windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }


-- ----------------------------------------------------
-- 定义我们的游戏引擎主程序项目
-- ----------------------------------------------------
-- project "GameEngine"
--     kind "ConsoleApp"
--     language "C++"
--     cppdialect "C++17"
--
--     -- 包含我们自己的源文件
--     files { "src/**.cpp" }
--
--     -- 关键：包含 Lua 库的头文件目录
--     -- %{wks.location} 指的是 workspace 所在目录，也就是项目根目录
--     -- 或者使用 Premake 的 token %{IncludeDir.Lua} 也可以
--     includedirs {
--         "vendor/lua/src"
--     }
--
--     -- 关键：链接到我们上面定义的 Lua 静态库项目
--     links { "Lua" }
--
--     -- 定义 Debug 和 Release 配置下的不同设定
--     filter "configurations:Debug"
--         defines { "DEBUG" }
--         symbols "On"
--
--     filter "configurations:Release"
--         defines { "NDEBUG" }
--         optimize "On"
--
