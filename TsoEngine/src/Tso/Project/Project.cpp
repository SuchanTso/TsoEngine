#include "TPch.h"
#include "Project.h"
#include "ProjectSerielizer.h"
#include "Scripting/ScriptingEngine.h"
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace Tso {

	Ref<Project> Project::New()
	{
		s_Project = CreateRef<Project>();
		return s_Project;
	}

	Ref<Project> Project::LoadProject(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();

		ProjectSerielizer serializer(project);
        project->m_ProjectDirectory = path.parent_path();
        s_Project = project;
		if (serializer.Deserieleze(path.string()))
		{
            std::string scriptPath = project->m_ProjectDirectory / project->GetConfig().ScriptModulePath;
            if((!scriptPath.empty()) && std::filesystem::exists(scriptPath)){
                ScriptingEngine::SetLuaPackagePath(scriptPath);
                ScriptingEngine::LoadAllScripts(scriptPath , false);
            }
			return s_Project;
		}

		return nullptr;
	}



    std::string Project::GetResourcePath() {
    #ifdef __APPLE__
        // 在 macOS 上，资源在 .app 包的 Contents/Resources/ 目录中
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        if (!mainBundle) {
            return "./"; // 如果获取失败，返回当前目录作为备用
        }
        CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
        char path[PATH_MAX];
        if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) {
            // 如果转换路径失败
            CFRelease(resourcesURL);
            return "./";
        }
        CFRelease(resourcesURL);
        return std::string(path) + "/";
    #else
        // 在 Windows 和 Linux 上，通常资源和可执行文件在同一目录或子目录中
        // 这里我们简单地返回当前相对路径，可以根据需要扩展
        return "./";
    #endif
    }

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		ProjectSerielizer serializer(s_Project);
		if (serializer.Serielize(path.string()))
		{
			s_Project->m_ProjectDirectory = path.parent_path();
			return true;
		}

		return false;
	}

}
