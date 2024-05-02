#include "TPch.h"
#include "Project.h"
#include "ProjectSerielizer.h"

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
		if (serializer.Deserieleze(path.string()))
		{
			project->m_ProjectDirectory = path.parent_path();
			s_Project = project;
			return s_Project;
		}

		return nullptr;
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