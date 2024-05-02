#pragma once
#include "Tso/Core/Core.h"
#include "Project.h"

namespace Tso {

	class ProjectSerielizer {
	public:
		ProjectSerielizer() = delete;
		ProjectSerielizer(Ref<Project> project);
		~ProjectSerielizer() = default;

		bool Serielize(const std::string& prjPath);
		bool Deserieleze(const std::string& prjPath);

	private:
		Ref<Project> m_Project = nullptr;
	};


}