#include "TPch.h"
#include "ScriptGlue.h"
#include "ScriptingEngine.h"

#include "glm/glm.hpp"
#include "Tso/Core/UUID.h"
#include "Tso/Scene/Scene.h"
#include "Tso/Scene/Component.h"
#include "Tso/Scene/Entity.h"

namespace Tso {


//	namespace Utils {
//
//    static UUID GetUUIDFromLua(lua_State* L){
//        std::string uuid_str = luaL_checkstring(L, 1);
//        uint64_t uuid = strtoull(uuid_str.c_str() , NULL , 0);
//        TSO_CORE_TRACE("Translate uuid_str[{}] -> [{}]", uuid_str , uuid);
//        return UUID(uuid);
//    }
//    // 模板化的组件指针 userdata 创建函数
//    template<typename T>
//    void PushComponentPointer(lua_State* L, T* component, const char* metatableName) {
//        if (!component) {
//            lua_pushnil(L);
//            return;
//        }
//        T** component_ptr_userdata = (T**)lua_newuserdata(L, sizeof(T*));
//        *component_ptr_userdata = component;
//        luaL_getmetatable(L, metatableName);
//        lua_setmetatable(L, -2);
//    }
//
//}
//static std::unordered_map<std::string , std::function<void(lua_State*, Entity*)>> ComponentsStr = {
//    {"TransformComponent" ,    [](lua_State* L, Entity* entity) {
//        if (entity->HasComponent<TransformComponent>()) {
//            Utils::PushComponentPointer(L, &entity->GetComponent<TransformComponent>(), "TsoEngine.TransformComponent");
//        } else {
//            lua_pushnil(L);
//        }
//    }},
//    {"Renderable" ,    [](lua_State* L, Entity* entity) {
//        if (entity->HasComponent<Renderable>()) {
//            Utils::PushComponentPointer(L, &entity->GetComponent<Renderable>(), "TsoEngine.Renderable");
//        } else {
//            lua_pushnil(L);
//        }
//    }}
//};
////==============================Bound Functions======================================================================
//namespace Global{
//    static int Log(lua_State* L) {
//        // 获取第一个参数作为字符串
//        const char* message = luaL_checkstring(L, 1);
//        TSO_CORE_TRACE("[LUA] {}", message);
//        return 0; // 0个返回值
//    }
//
//    static int IsKeyPressed(lua_State* L) {
//        int keycode = static_cast<int>(luaL_checkinteger(L, 1));
//        bool keyDown = Input::IsKeyPressed(keycode);
//        lua_pushboolean(L, keyDown);
//        return 1;
//    }
//
//    static int GetEntityByUUID(lua_State* L){
//        
//        Scene* scene = ScriptingEngine::GetSceneContext();
//        if (!scene) {
//            lua_pushnil(L);
//            return 1;
//        }
//        UUID uuid = Utils::GetUUIDFromLua(L);
//        Entity entity = scene->GetEntityByUUID(uuid);
//        if ((bool)entity) {
//            // 将 C++ Entity 对象作为 userdata 推给 Lua
//            Entity* user_data = (Entity*)lua_newuserdata(L, sizeof(Entity));
//            new(user_data) Entity(entity);
//            luaL_getmetatable(L, "TsoEngine.Entity");
//            lua_setmetatable(L, -2);
//        } else {
//            TSO_CORE_ERROR("didn't get entity:[{}]" , uuid);
//            lua_pushnil(L);
//        }
//        return 1; // 1个返回值 (entity userdata 或 nil)
//    }
//}
//namespace EntityMethod{
//
//
//    static int GetComponent(lua_State* L){
//        Entity* self = (Entity*)luaL_checkudata(L, 1, "TsoEngine.Entity");
//        self->SetScene(ScriptingEngine::GetSceneContext());
//        std::string componentName = luaL_checkstring(L, 2);
//        std::string cppMetatableName = "TsoEngine." + componentName;
//        auto it = ComponentsStr.find(componentName);
//        if(it == ComponentsStr.end()){
//            lua_pushnil(L);
//        }
//        else{
//            it->second(L, self);
//            std::string modulePath = "core.";
//            modulePath += componentName;
//            lua_getglobal(L, "require");
//            lua_pushstring(L, modulePath.c_str());
//            if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
//                TSO_CORE_WARN("Failed to execute Lua module '{}': {}", modulePath, lua_tostring(L, -1));
//                lua_pop(L, 1);
//                lua_pushnil(L);
//                return 1; // 返回原始 userdata
//            }
//
//            if (!lua_istable(L, -1)) {
//                lua_pop(L, 1);
//                lua_pushnil(L);
//                return 1; // 模块不存在，返回原始 userdata
//            }
//            
//            // 此时栈: [..., cpp_userdata, lua_module_table]
//
//            // 5. 设置原型链
//            luaL_getmetatable(L, cppMetatableName.c_str()); // 获取 C++ 元表
//            lua_setmetatable(L, -2); // setmetatable(lua_module_table, cpp_metatable)
//            lua_setmetatable(L, -2); // setmetatable(cpp_userdata, lua_module_table)
//            
//            return 1;
//        }
//        return 1;
//    }
//}
//// --- TransformComponent 方法 ---
//namespace TransformComponentMethods {
//    // 辅助函数，避免重复
//    static TransformComponent* GetSelf(lua_State* L) {
//        TransformComponent** self_ptr = (TransformComponent**)lua_touserdata(L, 1);
//        return *self_ptr;
//    }
//
//    static int GetPosition(lua_State* L) {
//        TransformComponent* self = GetSelf(L);
//        if (!self) return 0;
//        Utils::PushComponentPointer(L, &self->m_Translation, "TsoEngine.Vector3"); // 返回一个可变的 vec3 指针
//        return 1;
//    }
//
//    static int SetPosition(lua_State* L) {
//        TransformComponent* self = GetSelf(L);
//        if (!self) return 0;
//        glm::vec3** vec3_ptr = (glm::vec3**)luaL_checkudata(L, 2, "TsoEngine.Vector3");
//        self->m_Translation = **vec3_ptr;
//        return 0;
//    }
//}
//
//namespace RenderableMethods{
//static Renderable* GetSelf(lua_State* L) {
//        Renderable** self_ptr = (Renderable**)lua_touserdata(L, 1);
//        return *self_ptr;
//    }
//static void Recalculate(Renderable* render){
//    render->subTexture->RecalculateCoords(render->spriteSize, render->textureIndex, render->textureSize);
//}
//
//    static int GetSubtextureIndex(lua_State* L){
//        Renderable* self = GetSelf(L);
//        Utils::PushComponentPointer(L, &self->textureIndex, "TsoEngine.Vector2");
//        Recalculate(self);
//        return 1;
//    }
//}
//
//// --- Vector3 方法 ---
//namespace Vector3Methods {
//    // 使得 Lua 可以写 vec3.x, vec3.y, vec3.z
//    static int get(lua_State* L) {
//        glm::vec3** self_ptr = (glm::vec3**)luaL_checkudata(L, 1, "TsoEngine.Vector3");
//        const char* key = luaL_checkstring(L, 2);
//        if (strcmp(key, "x") == 0) lua_pushnumber(L, (*self_ptr)->x);
//        else if (strcmp(key, "y") == 0) lua_pushnumber(L, (*self_ptr)->y);
//        else if (strcmp(key, "z") == 0) lua_pushnumber(L, (*self_ptr)->z);
//        else lua_pushnil(L);
//        return 1;
//    }
//
//    static int set(lua_State* L) {
//        glm::vec3** self_ptr = (glm::vec3**)luaL_checkudata(L, 1, "TsoEngine.Vector3");
//        const char* key = luaL_checkstring(L, 2);
//        float value = luaL_checknumber(L, 3);
//        if (strcmp(key, "x") == 0) (*self_ptr)->x = value;
//        else if (strcmp(key, "y") == 0) (*self_ptr)->y = value;
//        else if (strcmp(key, "z") == 0) (*self_ptr)->z = value;
//        return 0;
//    }
//}
//
//namespace Vector2Methods {
//    // 使得 Lua 可以写 vec2.x, vec2.y
//    static int get(lua_State* L) {
//        glm::vec2** self_ptr = (glm::vec2**)luaL_checkudata(L, 1, "TsoEngine.Vector2");
//        const char* key = luaL_checkstring(L, 2);
//        if (strcmp(key, "x") == 0) lua_pushnumber(L, (*self_ptr)->x);
//        else if (strcmp(key, "y") == 0) lua_pushnumber(L, (*self_ptr)->y);
//        else lua_pushnil(L);
//        return 1;
//    }
//
//    static int set(lua_State* L) {
//        glm::vec2** self_ptr = (glm::vec2**)luaL_checkudata(L, 1, "TsoEngine.Vector2");
//        const char* key = luaL_checkstring(L, 2);
//        float value = luaL_checknumber(L, 3);
//        if (strcmp(key, "x") == 0) (*self_ptr)->x = value;
//        else if (strcmp(key, "y") == 0) (*self_ptr)->y = value;
//        return 0;
//    }
//}
//==============================Bound Functions======================================================================
//	void ScriptGlue::RegisterFunctions() {
//        lua_State* L = ScriptingEngine::GetLuaState();
//
//        // === 1. global functions registry ===
//        lua_newtable(L);
//        
//        static const luaL_Reg world_funcs[] = {
//            //global functions
//            {"GetEntityByUUID", Global::GetEntityByUUID},
//            {"Log",             Global::Log},
//            {"IsKeyPressed",    Global::IsKeyPressed},
//            {NULL, NULL}
//        };
//        
//        luaL_setfuncs(L, world_funcs, 0);
//        lua_setglobal(L, "World");
//        
//        // === 2. entity registry ===
//
//        luaL_newmetatable(L, "TsoEngine.Entity");
//        lua_pushcfunction(L, [](lua_State* L){ // __gc method
//            Entity** e = (Entity**)lua_touserdata(L, 1);
//            (*e)->~Entity(); // 调用析构
//            return 0;
//        });
//        lua_setfield(L, -2, "__gc");
//        lua_pushvalue(L, -1);
//        lua_setfield(L, -2, "__index");
//        luaL_Reg entity_methods[] = {
//            {"GetComponent", EntityMethod::GetComponent},
//            {NULL, NULL}
//        };
//        luaL_setfuncs(L, entity_methods, 0);
//        lua_pop(L, 1);
//        
//        //TransformComponent
//        luaL_newmetatable(L, "TsoEngine.TransformComponent");
//        lua_pushvalue(L, -1);
//        lua_setfield(L, -2, "__index");
//        luaL_Reg transform_methods[] = {
//            {"GetPosition", TransformComponentMethods::GetPosition},
//            {"SetPosition", TransformComponentMethods::SetPosition},
//            {NULL, NULL}};
//        luaL_setfuncs(L, transform_methods, 0);
//        lua_pop(L, 1);
//    
//        //Renderable
//        luaL_newmetatable(L, "TsoEngine.Renderable");
//        lua_pushvalue(L, -1);
//        lua_setfield(L, -2, "__index");
//        luaL_Reg renderable_methods[] = {
//            {"GetSubtextureIndex", RenderableMethods::GetSubtextureIndex},
//            {NULL, NULL}};
//        luaL_setfuncs(L, renderable_methods, 0);
//        lua_pop(L, 1);
//        
//        //Vector2
//        luaL_newmetatable(L, "TsoEngine.Vector2");
//        lua_pushcfunction(L, Vector2Methods::get); lua_setfield(L, -2, "__index");
//        lua_pushcfunction(L, Vector2Methods::set); lua_setfield(L, -2, "__newindex");
//        lua_pop(L, 1);
//    
//        // Vector3
//        luaL_newmetatable(L, "TsoEngine.Vector3");
//        lua_pushcfunction(L, Vector3Methods::get); lua_setfield(L, -2, "__index");
//        lua_pushcfunction(L, Vector3Methods::set); lua_setfield(L, -2, "__newindex");
//        lua_pop(L, 1);
//    
//	}
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
        "spriteIndex", sol::property(
            // Getter
            [](const Renderable& r) { return r.textureIndex; },
            // Setter
            [](Renderable& r, const glm::vec2& index) {
                r.textureIndex = index;
                // [MODIFIED] 业务逻辑也封装在绑定中
                r.subTexture->RecalculateCoords(r.spriteSize, r.textureIndex, r.textureSize);
            }
        )
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
            // ... 在这里添加更多 else if ...
            
            TSO_CORE_WARN("Lua script trying to get unknown or missing component: {}", componentName);
            return sol::lua_nil;
        }
    );
}

}
