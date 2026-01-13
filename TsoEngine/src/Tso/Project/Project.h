#pragma once
#include "Tso/Core/Log.h"
#include "Tso/Core/Core.h"
#include <filesystem>

namespace Tso {

	struct ProjectConfig {
		std::string Name = "Untitled";
		std::filesystem::path FirstScene;
		std::filesystem::path AssetPath;
		std::filesystem::path ScriptModulePath;//TODO: make it realise in script engine
        std::filesystem::path ProjectPath;
        std::vector<std::filesystem::path> Scenes;
	};

	class Project {
	public:
		static Ref<Project> New();
		static Ref<Project> LoadProject(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
		ProjectConfig& GetConfig() { return m_Config; }
        static void SetProjectPath(const std::string& projPath){
            TSO_CORE_ASSERT(s_Project != nullptr, "there is no project opening");
            s_Project->m_Config.ProjectPath = std::filesystem::path(projPath);
        }
        static std::filesystem::path& GetProjectPath(){
            TSO_CORE_ASSERT(s_Project != nullptr, "there is no project opening");
            return s_Project->m_Config.ProjectPath;
        }
        
        static void SetSceneAsset(const std::vector<std::filesystem::path>& scenes){
            TSO_CORE_ASSERT(s_Project != nullptr, "there is no project opening");
            s_Project->m_Config.Scenes = scenes;
        }
        
        static std::vector<std::filesystem::path>& GetSceneAsset(){
            TSO_CORE_ASSERT(s_Project != nullptr, "there is no project opening");
            return s_Project->m_Config.Scenes;
        }
        
        
        static void CloseActive(){s_Project = nullptr;}

		static std::filesystem::path& GetProjectDirectory() {
			TSO_CORE_ASSERT(s_Project != nullptr, "there is no project opening");
			return s_Project->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory() {
			TSO_CORE_ASSERT(s_Project != nullptr, "there is no project opening");
			return GetProjectDirectory() / s_Project->m_Config.AssetPath;
		}

		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& filePath) {
			return GetProjectDirectory() / filePath; 
		}
        static std::string GetResourcePath();

		static Ref<Project> GetActive() { return s_Project; }
	private:
		inline static Ref<Project> s_Project;
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;
	};
}
