#include "TPch.h"
#include "ScriptGlue.h"
#include "ScriptingEngine.h"

#include "glm/glm.hpp"
#include "Tso/Core/UUID.h"
#include "Tso/Scene/Scene.h"
#include "Tso/Scene/Component.h"
#include "Tso/Scene/Entity.h"
#include "Network/ByteStream.h"
#include "Network/NetworkEngine.h"
#include "Tso/Protocol/LuaBridge.h"
#include "Tso/Project/Resource.h"
#include "Tso/Scripting/ScriptTaskManager.h"

namespace Tso {

void ScriptGlue::RegisterFunctions() {
    // 获取 sol::state 的引用
    sol::state& lua = ScriptingEngine::GetLuaState();

    //======================================================================================
    // === 1. 绑定全局函数和命名空间 ===
    //======================================================================================

    // 创建一个名为 "World" 的全局表 (命名空间)
    sol::table world = lua.create_table("World");

    // 绑定全局函数
    world["Log"] = [](const std::string& message) {
        TSO_TRACE("[LUA] {}", message);
    };

    world["IsKeyPressed"] = &Input::IsKeyPressed;
    
    world["CreateEntity"] = [](sol::optional<std::string> tag) -> Entity {
            Scene* scene = ScriptingEngine::GetSceneContext();
            if (!scene) {
                TSO_CORE_ERROR("No scripting context when creating entity");
                return Entity{}; // 返回无效实体
            }
            if (tag) {
                return scene->CreateEntity(tag.value());
            }
            return scene->CreateEntity();
        };

    world["GetEntityByUUID"] = [](const std::string& uuid_str) -> sol::object {
        Scene* scene = ScriptingEngine::GetSceneContext();
        if (!scene) return sol::lua_nil;

        // [MODIFIED] 直接在绑定中处理字符串到UUID的转换
        uint64_t uuid_val = strtoull(uuid_str.c_str(), NULL, 0);
        Entity entity = scene->GetEntityByUUID(uuid_val);

        if (entity) {
            // sol 会自动将 C++ 对象包装成 userdata
            return sol::make_object(ScriptingEngine::GetLuaState(), entity);
        }
        return sol::lua_nil;
    };
    world["FindEntityByName"] = [](const std::string& name) -> sol::object {
        Scene* scene = ScriptingEngine::GetSceneContext();
        if (!scene) return sol::lua_nil;

        // [MODIFIED] 直接在绑定中处理字符串到UUID的转换
        auto tagView = scene->GetAllEntitiesWith<TagComponent>(true);
        for(auto e : tagView){
            auto& tag = e.GetComponent<TagComponent>();
            if(tag.m_Name == name){
                return sol::make_object(ScriptingEngine::GetLuaState(), e);
            }
        }
        return sol::lua_nil;
    };
    world["RunLater"] = [](sol::protected_function func, float delay) {
        ScriptTaskManager::Get().AddTask(func, delay);
    };


    //======================================================================================
    // === 2. 绑定 C++ 类 (usertype) ===
    //======================================================================================

    // --- 基础类型绑定 ---

    // 绑定 glm::vec2
//     'usertype' 会自动创建元表 "Vector2"
    lua.new_usertype<glm::vec2>("Vector2",
        // 构造函数保持不变
        sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>(),
        // 使用 lambda 来明确指定 getter 和 setter
        "x", sol::property(
            [](const glm::vec2& v) { return v.x; }, // Getter
            [](glm::vec2& v, float x) { v.x = x; }  // Setter
        ),
        "y", sol::property(
            [](const glm::vec2& v) { return v.y; },
            [](glm::vec2& v, float y) { v.y = y; }
        )
        // 你还可以在这里添加方法，例如
        // "length", [](const glm::vec2& v) { return glm::length(v); }
    );

    // [MODIFIED] 绑定 glm::vec3，同样使用 lambda 包装
    lua.new_usertype<glm::vec3>("Vector3",
        sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
        "x", sol::property(
            [](const glm::vec3& v) { return v.x; },
            [](glm::vec3& v, float x) { v.x = x; }
        ),
        "y", sol::property(
            [](const glm::vec3& v) { return v.y; },
            [](glm::vec3& v, float y) { v.y = y; }
        ),
        "z", sol::property(
            [](const glm::vec3& v) { return v.z; },
            [](glm::vec3& v, float z) { v.z = z; }
        )
    );
    
    lua.new_usertype<glm::vec4>("Vector4",
        sol::constructors<glm::vec4(), glm::vec4(float), glm::vec4(float, float, float, float)>(),
        "x", sol::property(
            [](const glm::vec4& v) { return v.x; },
            [](glm::vec4& v, float x) { v.x = x; }
        ),
        "y", sol::property(
            [](const glm::vec4& v) { return v.y; },
            [](glm::vec4& v, float y) { v.y = y; }
        ),
        "z", sol::property(
            [](const glm::vec4& v) { return v.z; },
            [](glm::vec4& v, float z) { v.z = z; }
        ),
        "w", sol::property(
            [](const glm::vec4& v) { return v.w; },
            [](glm::vec4& v, float w) { v.w = w; }
        )
    );


    // --- 组件绑定 ---

    // 绑定 TransformComponent
    lua.new_usertype<TransformComponent>("TransformComponent",
        // 使用 sol::property 可以让方法调用看起来像变量访问
        // 在Lua中写 transform.position 时，会调用 GetPosition
        // 写 transform.position = myVec3 时，会调用 SetPosition
        // [MODIFIED] 假设 TransformComponent 有 Get/SetPosition 方法
        "position", sol::property(&TransformComponent::GetPos, &TransformComponent::SetPos)

    );


    // 绑定 Renderable
    lua.new_usertype<Renderable>("Renderable",
        "color", sol::property(
            // Getter
           [](const Renderable& r) { return r.m_Color; },
            // Setter
            [](Renderable& r, const glm::vec4& color) {
                r.m_Color = color;
            }
        ),
         "is_subtexture" , sol::property(
             [](const Renderable& r){return r.isSubtexture;},
             [](Renderable& r , bool isSubtexture){
                 r.isSubtexture = isSubtexture;
             }
         ),
         "type" , sol::property(
            [](const Renderable& r){return r.type;},
            [](Renderable& r , uint8_t type){
                r.type = RenderType(type);
                
             }
         ),
         "sprite_index" , sol::property(
        [](const Renderable& r){return r.textureIndex;},
        [](Renderable& r , glm::vec2& index){
                r.textureIndex = index;
            if(r.subTexture){
                r.subTexture->RecalculateCoords(r.spriteSize, r.textureIndex, r.textureSize);
            }
             }
         ),
         "sprite_size" , sol::property(
       [](const Renderable& r){return r.spriteSize;},
        [](Renderable& r , glm::vec2& spriteSize){
                r.spriteSize = spriteSize;
            if(r.subTexture){
                r.subTexture->RecalculateCoords(r.spriteSize, r.textureIndex, r.textureSize);
            }
             }
         ),
         "texture_size" , sol::property(
        [](const Renderable& r){return r.textureSize;},
        [](Renderable& r , glm::vec2& texture_size){
            r.textureSize = texture_size;
            if(r.subTexture){
                r.subTexture->RecalculateCoords(r.spriteSize, r.textureIndex, r.textureSize);
            }
            }
         ),
         "texture_path" , sol::property(
        [](const Renderable& r){return "";},
        [](Renderable& r , std::string texturePath){
            auto path = Project::GetActive()->GetResourcePath() + "/" + texturePath;
            auto texture = Resource::GetTextureByPath(path);
            if(texture){
                r.subTexture = SubTexture2D::CreateByCoord(texture, r.spriteSize, r.textureIndex, r.textureSize);
            }
             }
         )
    );
    lua.new_usertype<IDComponent>("UUIDComponent",
        "uuid", sol::property(
            // Getter
              [](const IDComponent& uuid) { return std::to_string(uuid.ID); },
            // Setter
            [](IDComponent& uuidComponent, const std::string& uuid) {
                TSO_CORE_WARN("setting uuid to an entity, which is unlikely to do so. if u are sure about this, ignore this warning");
                uuidComponent.ID = strtoull(uuid.c_str(), NULL, 10);
            }
        )
    );
    
    lua.new_usertype<TextComponent>("TextComponent",
        "Text", sol::property(
            // Getter
            [](const TextComponent& t) { return t.Text; },
            // Setter
            [](TextComponent& t, const std::string& text) {
                t.Text = text;
                // [MODIFIED] 业务逻辑也封装在绑定中
            }
        )
    );

    lua.new_usertype<UITransformComponent>("UITransformComponent",
        "Position", sol::property(
            // Getter
             [](const UITransformComponent& ui) { return ui.UIpos; },
            // Setter
            [](UITransformComponent& ui, const glm::vec2& pos) {
                ui.UIpos = pos;
                // [MODIFIED] 业务逻辑也封装在绑定中
            }
        ),
       "Size", sol::property(
           // Getter
           [](const UITransformComponent& ui) { return ui.UISize; },
           // Setter
           [](UITransformComponent& ui, const glm::vec2& size) {
               ui.UISize = size;
               // [MODIFIED] 业务逻辑也封装在绑定中
           }
       )
    );
    lua.new_usertype<InputFieldComponent>("InputFieldComponent",
        "Text", sol::property(
            // Getter
            [](const InputFieldComponent& c) { return c.Text; },
            // Setter
            [](InputFieldComponent& c, const std::string& text) {
                c.Text = text;
                // [MODIFIED] 业务逻辑也封装在绑定中
            }
        )
    );
    lua.new_usertype<ButtonComponent>("ButtonComponent",
        "SetOnClick", [](ButtonComponent& self, sol::function func) {
            self.OnClick = func;
        }
    );

    // --- 核心类绑定 ---

    // 绑定 Entity
    lua.new_usertype<Entity>("Entity",
        sol::constructors<Entity()>(),
        // [MODIFIED] 移除静态 map，直接在 lambda 中处理
        "GetComponent", [](Entity& entity, const std::string& componentName) -> sol::object {
            entity.SetScene(ScriptingEngine::GetSceneContext());

            if (componentName == "TransformComponent") {
                if (entity.HasComponent<TransformComponent>()) {
                    return sol::make_object(ScriptingEngine::GetLuaState(), &entity.GetComponent<TransformComponent>());
                }
            }
            else if (componentName == "Renderable") {
                if (entity.HasComponent<Renderable>()) {
                    return sol::make_object(ScriptingEngine::GetLuaState(), &entity.GetComponent<Renderable>());
                }
            }
            else if(componentName == "UITransformComponent"){
                if(entity.HasComponent<UITransformComponent>()){
                    return sol::make_object(ScriptingEngine::GetLuaState(), &entity.GetComponent<UITransformComponent>());

                }
            }
            else if(componentName == "InputFieldComponent"){
                if(entity.HasComponent<InputFieldComponent>()){
                    return sol::make_object(ScriptingEngine::GetLuaState(), &entity.GetComponent<InputFieldComponent>());
                }
            }
            else if(componentName == "TextComponent"){
                if(entity.HasComponent<TextComponent>()){
                    auto& tc = entity.GetComponent<TextComponent>();
                    if(!tc.isUI){
                        return sol::make_object(ScriptingEngine::GetLuaState(), &entity.GetComponent<TextComponent>());
                    }
                }
            }
            else if(componentName == "UITextComponent"){
                if(entity.HasComponent<TextComponent>()){
                    auto& tc = entity.GetComponent<TextComponent>();
                    if(tc.isUI){
                        return sol::make_object(ScriptingEngine::GetLuaState(), &entity.GetComponent<TextComponent>());
                    }
                }
                else{
                    entt::entity eid = (entt::entity)entity.GetEntityID();
                    auto en = Entity(eid , ScriptingEngine::GetSceneContext());
                    if(en.HasComponent<IDComponent>()){
                        auto uuid = en.GetUUID();
                    }
                    TSO_WARN("entity doesn't has UITextComponent");
                }
            }
            else if(componentName == "UUIDComponent"){
                return sol::make_object(ScriptingEngine::GetLuaState(), &entity.GetComponent<IDComponent>());
            }
            else if(componentName == "ImageView"){
                if(entity.HasComponent<Renderable>()){
                    auto& r = entity.GetComponent<Renderable>();
                    if(r.uiview){
                        return sol::make_object(ScriptingEngine::GetLuaState(), &entity.GetComponent<Renderable>());
                    }
                }
                return sol::lua_nil;
            }
            
            // ... 在这里添加更多 else if ...
            
            TSO_CORE_WARN("Lua script trying to get unknown or missing component: {}", componentName);
            return sol::lua_nil;
        },
         "IsValid", [](Entity& entity){
            bool res = entity.HasComponent<IDComponent>();
            if(!res){
                TSO_WARN("entity {} is not valid" , entity.GetEntityID());
            }
        return res;
    },
         "Destroy", [](Entity& entity) {
             Scene* scene = ScriptingEngine::GetSceneContext();
//            TSO_INFO("entity {} destroyed itself" , uint32_t(entity));
             if (scene) scene->DeleteEntity(entity);
             else{
                 TSO_CORE_ERROR("Didn't get scripting engine context when destroying entity");
             }
         },
         "GetTag", [](Entity& entity) ->std::string{
        if(entity.HasComponent<TagComponent>()){
            return entity.GetComponent<TagComponent>().m_Name;
        }
        else{
            TSO_WARN("failed to get tag component for entity");
            return "";
        }
//            return sol::make_object(ScriptingEngine::GetLuaState(), &entity.GetComponent<TagComponent>().m_Name);
        },
         "SetActive", [](Entity& entity , bool active) ->void{
        TSO_INFO("Set entity : {} as {}" , entity.GetUUID() , active);
            entity.GetComponent<ActiveComponent>().Active = active;
        },
                             
         "AddComponent", [](Entity& entity, const std::string& componentName, sol::optional<sol::table> initial_values) -> sol::object {
             // 在这里你需要一个组件注册表或一长串 if/else
             if (componentName == "Script") {
                 auto& sc = entity.AddComponent<ScriptComponent>();
//                 sc.ClassName = initial_values.get<std::string>("ScriptName");
                 if (initial_values) {
                     sc.ClassName = (*initial_values).get_or("ScriptName", std::string(""));
                 }
                 ScriptingEngine::OnCreateEntity(entity);
                 // ScriptingEngine 会在下一帧的 OnCreateEntity 中处理这个组件
                 return sol::make_object(ScriptingEngine::GetLuaState(), &sc);
             }
             else if(componentName == "UITextComponent"){
                 auto& tc = entity.AddComponent<TextComponent>();
                 if(!entity.HasComponent<UITransformComponent>()){
                     entity.AddComponent<UITransformComponent>();
                 }
                 if (initial_values) {
                     tc.Text = (*initial_values).get_or("Text", std::string(""));
                 }
                 tc.isUI = true;
                 return sol::make_object(ScriptingEngine::GetLuaState(), &tc);
             }
             else if(componentName == "InputFieldComponent"){
                 auto& ic = entity.AddComponent<InputFieldComponent>();
                 if(!entity.HasComponent<UITransformComponent>()){
                     entity.AddComponent<UITransformComponent>();
                 }
                 auto&tc = entity.AddComponent<TextComponent>();
                 tc.isUI = true;
                 return sol::make_object(ScriptingEngine::GetLuaState(), &ic);
             }
             else if(componentName == "ButtonComponent"){
                 auto& bc = entity.AddComponent<ButtonComponent>();
                 if(!entity.HasComponent<UITransformComponent>()){
                     entity.AddComponent<UITransformComponent>();
                 }
                 return sol::make_object(ScriptingEngine::GetLuaState(), &bc);
             }
             else if(componentName == "ImageView"){
                 if(!entity.HasComponent<Renderable>()){
                     entity.AddComponent<Renderable>();
                 }
                 if(!entity.HasComponent<UITransformComponent>()){
                     entity.AddComponent<UITransformComponent>();
                 }
                 auto& r = entity.GetComponent<Renderable>();
                 r.uiview = true;
                 return sol::make_object(ScriptingEngine::GetLuaState(), &r);
             }
             else if(componentName == "CameraComponent"){
                 if(!entity.HasComponent<CameraComponent>()){
                     auto& camera = entity.AddComponent<CameraComponent>();
                     if (initial_values) {
                         camera.m_Pramiary = (*initial_values).get_or("isMain", false);
                     }
                     return sol::make_object(ScriptingEngine::GetLuaState(), &camera);
                 }
             }
        
             // ... 其他组件 ...
             return sol::lua_nil;
         },
         "GetScript", [](Entity& entity) -> sol::object {
                     auto instance = ScriptingEngine::GetScriptInstance(entity.GetUUID());
                     if (instance) {
                         return instance->GetLuaInstance();
                     }
                     return sol::lua_nil;
                 }
    );
    
    // --- 绑定 ByteStream (现在同时用于发送和接收) ---
        lua.new_usertype<ByteStream>("ByteStream",
            // 构造函数
            sol::constructors<ByteStream()>(),

            // --- 用于接收 (Read) ---
            "ReadByte", &ByteStream::read<uint8_t>,
             "ReadULong", &ByteStream::read<uint64_t>,
             "ReadString", &ByteStream::readString,
            
            // --- 用于发送 (Write) ---
            // 我们需要返回 *this 来支持链式调用 (e.g., bs:WriteString(...):WriteByte(...))
            "WriteString", [](ByteStream& self, const std::string& str) -> ByteStream& {
            self.writeString(str);
                return self;
            },
            "WriteByte", [](ByteStream& self, uint8_t val) -> ByteStream& {
                self.write(val);
                return self;
            },
             "WriteBool", [](ByteStream& self, bool val) -> ByteStream& {
                 self.write(val);
                 return self;
             },

            // --- 获取最终数据用于发送 ---
            "GetData", [](const ByteStream& self) {
                // sol2 会自动将 const uint8_t* + size 转换为 lua_string
                return std::string_view(
                    reinterpret_cast<const char*>(self.getRawBuffer()),
                    self.getRawBufferLength()
                );
            }
        );
    
    sol::table network = lua.create_table("Network");
    network["Connect"] = &NetWorkEngine::Connect;
    network["DisConnected"] = &NetWorkEngine::DisConnect;
    network["IsConnected"] = &NetWorkEngine::IsConnect;

        // ...
        // [MODIFIED] Send现在直接接收 protocolId 和一个二进制字符串
    network["Send"] = [](uint16_t pid, const std::string& payload_str) {
        ByteStream payload((const uint8_t*)payload_str.data(), payload_str.length());
        Tso::NetWorkEngine::Send(pid, payload);
    };
    network["RegisterHandler"] = [](uint8_t moduleId, sol::function handler) {
            if (!handler.valid()) return;
            // 创建一个包装了Lua function的C++回调对象
        auto protocolHandler = CreateRef<LuaProtocolHandler>(ScriptingEngine::GetLuaState(), handler);
        ScriptingEngine::GetLuaProtocolHandlers()[moduleId] = protocolHandler;
            NetWorkEngine::RegisterHandler(moduleId, protocolHandler.get());
        };
        
        // [实现] UnregisterHandler
        network["UnregisterHandler"] = [](uint8_t moduleId) {
            NetWorkEngine::UnregisterHandler(moduleId);
            ScriptingEngine::GetLuaProtocolHandlers().erase(moduleId);
        };

}

}
