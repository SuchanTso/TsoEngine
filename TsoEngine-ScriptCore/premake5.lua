project "TsoEngine-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"
	
	targetdir ("Build")
	objdir ("Intermediates")

	files 
	{
		"Scripts/**.cs"
	}