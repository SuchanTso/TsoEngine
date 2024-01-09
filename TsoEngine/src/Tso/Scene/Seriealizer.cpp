#include "TPch.h"
#include "Seriealizer.h"
#include "Tso/Scene/Entity.h"
#include "Tso/Scene/Component.h"
#include <fstream>
#include "yaml-cpp/yaml.h"

namespace Tso {
	Seriealizer::Seriealizer(Scene* scene)
		:m_Scene(scene)
	{
	}


	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}


	void Tso::Seriealizer::SeriealizeScene(const std::string& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;

				SeriealizeEntity(out, entity);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	static void SeriealizeEntity(YAML::Emitter& out, Entity& entity)
	{
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetEntityID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto& tag = entity.GetComponent<TagComponent>().m_Name;
			out << YAML::Key << "Tag" << YAML::Value << tag.c_str();

			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& comp = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translate" << YAML::Value << comp.GetPos();
			out << YAML::Key << "Rotation" << YAML::Value << comp.GetRotate();
			out << YAML::Key << "Scale" << YAML::Value << comp.GetScale();

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CamermaComponent";
			out << YAML::BeginMap; // CameramComponent

			auto& comp = entity.GetComponent<CameraComponent>();
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)comp.m_Camera.GetProjectionType();
			out << YAML::Key << "ProjectionNearClip" << YAML::Value << comp.m_Camera.GetProjectionNearClip();
			out << YAML::Key << "ProjectionFarClip" << YAML::Value << comp.m_Camera.GetProjectionFarClip();
			out << YAML::Key << "ProjectionFov" << YAML::Value << comp.m_Camera.GetProjectionFov();

			out << YAML::Key << "OrthographicNear" << YAML::Value << comp.m_Camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << comp.m_Camera.GetOrthographicFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << comp.m_Camera.GetOrthographicSize();

			out << YAML::EndMap; // CameramComponent
		}
	}
}