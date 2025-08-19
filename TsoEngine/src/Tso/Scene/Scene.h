#pragma once
#include "entt.hpp"
#include "Tso/Core/TimeStep.h"
#include "Tso/Physics/CollideListener.h"
#include "Tso/Core/UUID.h"
#include "Tso/Core/Core.h"
#include "glm/glm.hpp"
class b2World;

namespace Tso {
	class Entity;
	class Font;
	class SceneCamera;
    class UISystem;
	class Scene {
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class Seriealizer;
	public:
		Scene();
        ~Scene();

		Entity CreateEntity(const std::string& name = "");
        
        Entity CreateEntityWithID(const UUID& uuid , const std::string& name = "");

		Entity GetEntityByUUID(const UUID& uuid);

		bool EntityExist(const UUID& uuid);
        
        void DeleteEntity(Entity entity);

		void OnScenePlay();

		void OnSceneStop();

		void OnUpdate(TimeStep ts);

		b2Body* CreatePhysicBody(Entity& entity);

		Entity CopyEntity(Entity entity);

		Ref<Entity> GetEntityByName(const std::string& name);

		void SetEntityParent(Entity& parent , Entity& child);

		void RemoveChild(Entity& parent , Entity& child);

		std::unordered_map<uint64_t, Ref<Entity>> GetEntityChildren(Entity& parent);

		Ref<Entity> GetEntityParent(Entity& child);

		SceneCamera* GetMainCamera();
        
        Ref<SceneCamera>GetUICamera(){return UICamera;}

        
        glm::mat4* GetMainCameraTransform(){return m_MainCameraTransform;}

		void SetSceneCamera(const Entity& cameraEntity);

		void SetUseSceneCamera(bool useSceneCamera);

		bool HasEntity(const UUID& uuid);
        
        template <typename... T>
        std::vector<Entity> GetAllEntitiesWith(){
            std::vector<Entity> res;
            auto view = m_Registry.view<T...>();
            
            // 预分配内存以提高性能 (可选)
//            res.reserve(view.size_hint());
            
            // [MODIFIED] 使用范围-for循环
            for (auto entity : view) {
                res.push_back({entity, this});
            }
                
            return res;
        }

	private:
		entt::registry m_Registry;

		float m_Time = 0.f;

		uint32_t m_EntityCount = 0;

		b2World* m_PhysicWorld = nullptr;

		NativeContactListener* m_PhysicsListener = nullptr;
        
		std::unordered_map<uint64_t, uint32_t> m_EntityMap;

		std::unordered_map<uint64_t, std::unordered_map<uint64_t, Ref<Entity>>> m_ChildrenMap;

		std::unordered_map<uint64_t, Ref<Entity>> m_ParentMap;

		bool m_Pause = true;

		bool m_UseSceneCamera = false;

		Ref<Entity> m_SceneCamera = nullptr;

		SceneCamera* mainCamera = nullptr;
        
        Ref<SceneCamera> UICamera = nullptr;
        
        glm::mat4* m_MainCameraTransform = nullptr;
        
        UISystem* m_UISystem = nullptr;


	};



}
