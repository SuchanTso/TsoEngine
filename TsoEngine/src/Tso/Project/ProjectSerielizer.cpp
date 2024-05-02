#include "TPch.h"
#include "ProjectSerielizer.h"
#include "yaml-cpp/yaml.h"
#include <fstream>


namespace Tso {
	ProjectSerielizer::ProjectSerielizer(Ref<Project> project)
		: m_Project(project)
	{
	}

	bool ProjectSerielizer::Serielize(const std::string& prjPath)
	{
		const auto& config = m_Project->GetConfig();

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;// Project
				out << YAML::Key << "Name" << YAML::Value << config.Name;
				out << YAML::Key << "StartScene" << YAML::Value << config.FirstScene.string();
				out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetPath.string();
				out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptModulePath.string();
				out << YAML::EndMap; // Project
			}
			out << YAML::EndMap; // Root
		}

		std::ofstream fout(prjPath);
		fout << out.c_str();

		return true;
	}

	bool ProjectSerielizer::Deserieleze(const std::string& prjPath)
	{
		auto& config = m_Project->GetConfig();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(prjPath);
		}
		catch (YAML::ParserException e)
		{
			TSO_CORE_ERROR("Failed to load project file '{0}'\n     {1}", prjPath, e.what());
			return false;
		}

		auto projectNode = data["Project"];
		if (!projectNode)
			return false;

		config.Name = projectNode["Name"].as<std::string>();
		config.FirstScene = projectNode["StartScene"].as<std::string>();
		config.AssetPath = projectNode["AssetDirectory"].as<std::string>();
		config.ScriptModulePath = projectNode["ScriptModulePath"].as<std::string>();
		return true;
	}
}