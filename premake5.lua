workspace "TsoEngine"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }
	startproject "TsoEditor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "third_party"
include "TsoEngine/third_party/GLFW"
include "TsoEngine/third_party/Glad"
include "TsoEngine/third_party/imgui"
include "TsoEngine/third_party/yaml-cpp"
include "TsoEngine/third_party/box2d"
group ""



group "Engine"
include "TsoEngine"
include "TsoEngine-ScriptCore"
group ""

group "Editor"
include "TsoEditor"
group ""

group "Misc"
include "Sandbox"
group ""
>>>>>>> 45c5b93 (fet：script engine)
