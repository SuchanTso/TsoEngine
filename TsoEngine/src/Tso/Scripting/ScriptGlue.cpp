#include "TPch.h"
#include "ScriptGlue.h"
#include "ScriptingEngine.h"

#include "glm/glm.hpp"
#include "Tso/Core/UUID.h"
#include "Tso/Scene/Scene.h"
#include "Tso/Scene/Component.h"

#include "mono/jit/jit.h"
#include "mono/metadata/tabledefs.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"


namespace Tso {

#define TSO_ADD_INTERNAL_FUNC(Name) mono_add_internal_call("Tso.InternalCalls::" #Name, Name)

	namespace Utils {

		static std::string MonoStringToString(MonoString* string)
		{
			char* cStr = mono_string_to_utf8(string);
			std::string str(cStr);
			mono_free(cStr);
			return str;
		}

	}

	static void NativeLOG(MonoString* msg) {
		std::string str = Utils::MonoStringToString(msg);
		TSO_CORE_INFO("{}", str.c_str());
	}

	static void GetTranslation(UUID uuid , glm::vec3* res) {
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity e = scene->GetEntityByUUID(uuid);
		auto& transform = e.GetComponent<TransformComponent>();
		*res = transform.m_Translation;
	}

	static void SetTranslation(UUID uuid, glm::vec3* res) {
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity e = scene->GetEntityByUUID(uuid);
		auto& transform = e.GetComponent<TransformComponent>();
		transform.m_Translation = *res;
	}


	static void GetRotationZ(UUID uuid, float* res) {
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity e = scene->GetEntityByUUID(uuid);
		auto& transform = e.GetComponent<TransformComponent>();
		*res = transform.m_Rotation.z;
	}

	static void SetRotationZ(UUID uuid, float res) {
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity e = scene->GetEntityByUUID(uuid);
		auto& transform = e.GetComponent<TransformComponent>();
		//TSO_CORE_TRACE("set rotationz = {}", res);
		transform.m_Rotation.z = res;
	}

	static void SetSpriteIndex(UUID uuid, glm::vec3* index) {
		//Fixme: there is an error when set a vec2 as a parameter
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity e = scene->GetEntityByUUID(uuid);
		auto& rc = e.GetComponent<Renderable>();
		if (rc.type == Texture && rc.isSubtexture) {
			rc.textureIndex = { index->x ,  index->y};
			rc.subTexture->RecalculateCoords(rc.spriteSize , rc.textureIndex , rc.textureSize);
		}
	}

	static void DestroyEntity(UUID uuid) {
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity e = scene->GetEntityByUUID(uuid);
		auto& activeC = e.GetComponent<ActiveComponent>();
		activeC.Active = false;
	}

	static void Fire(UUID uuid) {
		//just a temp func
		//this specified func is not flexible and is not allowed
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity e = scene->GetEntityByUUID(uuid);
		auto& tc = e.GetComponent<TransformComponent>();
		Entity bullet = scene->CreateEntity("bullet");
		auto& btc = bullet.GetComponent<TransformComponent>();
		btc = tc;
		auto&rigid = bullet.AddComponent<Rigidbody2DComponent>();
		auto& script = bullet.AddComponent<ScriptComponent>();
		script.ClassName = "Tso.Bullet";
		rigid.Type = Rigidbody2DComponent::BodyType::Dynamic;
		bullet.AddComponent<BoxCollider2DComponent>();
		auto& render = bullet.AddComponent<Renderable>();
		render.m_Color = glm::vec4(0.8f, 0.3f, 0.23f, 1.f);
		render.type = PureColor;
		auto body = scene->CreatePhysicBody(bullet);
		body->SetGravityScale(0.f);
		body->SetLinearVelocity(b2Vec2(10.f, 0.f));
		ScriptingEngine::OnCreateEntity(bullet);
	}

	static bool IsKeyPressed(int keycode) {
		return Input::IsKeyPressed(keycode);
	}

	void ScriptGlue::RegisterFunctions() {
		TSO_ADD_INTERNAL_FUNC(NativeLOG);

#pragma region Transform
		TSO_ADD_INTERNAL_FUNC(GetTranslation);
		TSO_ADD_INTERNAL_FUNC(SetTranslation);
		TSO_ADD_INTERNAL_FUNC(GetRotationZ);
		TSO_ADD_INTERNAL_FUNC(SetRotationZ);
#pragma endregion

#pragma region SpriteAnimation
		TSO_ADD_INTERNAL_FUNC(SetSpriteIndex);
#pragma endregion




		TSO_ADD_INTERNAL_FUNC(IsKeyPressed);

		TSO_ADD_INTERNAL_FUNC(DestroyEntity);

		TSO_ADD_INTERNAL_FUNC(Fire);


	}

}