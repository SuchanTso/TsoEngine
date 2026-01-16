//
//  Resource.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2025/8/29.
//
#include "TPch.h"
#include "Resource.h"
#include "Tso/Renderer/Texture.h"
#include "Tso/Renderer/Material.h"
#include "Tso/Animation/AnimationClip.h"
#include "Tso/Scripting/ScriptingEngine.h"

namespace Tso{
void Resource::Init(){
    s_Resource = CreateRef<Resource>();
    s_Resource->m_ResourceData = CreateRef<ResourceData>();
//    auto fontPath = Project::GetResourcePath() + "assets/TimesNewRoman.ttf";
//    s_Resource->m_ResourceData->font = CreateRef<Font>(fontPath);
}
void Resource::InitProject(){
    TSO_CORE_ASSERT(Project::GetActive() , "no project opening!");
    auto projectRoot = Project::GetProjectDirectory();
    std::filesystem::path assetsDir = projectRoot / "Assets";
    
    // 递归遍历
    for (auto& file : VirtualFileSystem::GetFiles(assetsDir)) {
        std::filesystem::path entry = std::filesystem::path(file);
        if (entry.extension() == ".meta") {
            // 读取 meta 里的 UUID
            UUID uuid = ReadUUIDFromMeta(entry);
            
            // 计算原始文件路径 (去掉 .meta 后缀)
            std::filesystem::path assetPath = entry;
            assetPath.replace_extension("");
            
            // 存入注册表
            s_Resource->m_ResourceData->resourceUUIDPathMap[uuid] = assetPath;
        }
    }
}

    UUID Resource::ReadUUIDFromMeta(const std::filesystem::path& path){
            Tso::Buffer fileBuffer = Tso::VirtualFileSystem::ReadFile(path);

            if (!fileBuffer.IsValid()) {
                TSO_CORE_ASSERT(false, "Failed to load file from VFS: {0}", path.string());
                return 0;
            }

            YAML::Node data;
            try {
                // [MODIFIED] 2. 从内存字符串加载
                // 注意：binary buffer 转 string，YAML::Load 需要一个标准字符串
                std::string yamlString(fileBuffer.DataPtr(), fileBuffer.Size());
                data = YAML::Load(yamlString);
                if (data["UUID"]) {
                    return data["UUID"].as<uint64_t>();
                }
            }
            catch (YAML::ParserException e) {
                TSO_CORE_ERROR("Failed to load Meta file '{0}'\n     {1}", path.string(), e.what());
                return 0;
            }
        return 0;
    }


Ref<Texture2D> Resource::GetTextureByPath( std::string& path){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->textures.find(path) == s_Resource->m_ResourceData->textures.end()){
        s_Resource->m_ResourceData->textures[path] = Texture2D::Create(path);
    }
    return s_Resource->m_ResourceData->textures[path];
}

static std::filesystem::path GetMetaPath(const std::filesystem::path& assetPath) {
        return assetPath.string() + ".meta";
    }

//===============================================GetResource============================================================
template<typename T>
Ref<T> Resource::GetResource(const UUID& uuid){
    if(auto res = GetResource<Texture2D>(uuid) != nullptr){
        return res;
    }
    if(auto res = GetResource<Shader>(uuid) != nullptr){
        return res;
    }
    if(auto res = GetResource<Material>(uuid) != nullptr){
        return res;
    }
    if(auto res = GetResource<AnimationClip>(uuid) != nullptr){
        return res;
    }
    if(auto res = GetResource<Font>(uuid) != nullptr){
        return res;
    }
    return nullptr;
}

template<>Ref<Shader>Resource::GetResource(const UUID& uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->shaderMap.find(uuid) == s_Resource->m_ResourceData->shaderMap.end()){
        if(s_Resource->m_ResourceData->resourceUUIDPathMap.find(uuid) != s_Resource->m_ResourceData->resourceUUIDPathMap.end()){
            auto path = s_Resource->m_ResourceData->resourceUUIDPathMap[uuid];
            auto shader = Shader::Create(path);
            auto name = std::filesystem::path(path).stem().string();
            AddResource<Shader>(name, uuid, shader);
            return shader;
        }
        TSO_CORE_WARN("Shader {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->shaderMap[uuid];
}

template<>Ref<Texture2D>Resource::GetResource(const UUID& uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->textureMap.find(uuid) == s_Resource->m_ResourceData->textureMap.end()){
        if(s_Resource->m_ResourceData->resourceUUIDPathMap.find(uuid) != s_Resource->m_ResourceData->resourceUUIDPathMap.end()){
            auto path = s_Resource->m_ResourceData->resourceUUIDPathMap[uuid];
            auto texture = Texture2D::Create(path);
            auto name = std::filesystem::path(path).stem().string();
            AddResource<Texture2D>(name, uuid, texture);
            return texture;
        }
        TSO_CORE_WARN("Texture {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->textureMap[uuid];
}

template<>Ref<Material>Resource::GetResource(const UUID& uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->materialMap.find(uuid) == s_Resource->m_ResourceData->materialMap.end()){
        if(s_Resource->m_ResourceData->resourceUUIDPathMap.find(uuid) != s_Resource->m_ResourceData->resourceUUIDPathMap.end()){
            auto path = s_Resource->m_ResourceData->resourceUUIDPathMap[uuid];
            auto material = Material::Create(path , uuid);
            auto name = std::filesystem::path(path).stem().string();
            AddResource<Material>(name, uuid, material);
            return material;
        }
        TSO_CORE_WARN("Material {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->materialMap[uuid];
}

template<>Ref<Font>Resource::GetResource(const UUID& uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->fontMap.find(uuid) == s_Resource->m_ResourceData->fontMap.end()){
        if(s_Resource->m_ResourceData->resourceUUIDPathMap.find(uuid) != s_Resource->m_ResourceData->resourceUUIDPathMap.end()){
            auto path = s_Resource->m_ResourceData->resourceUUIDPathMap[uuid];
            auto font = CreateRef<Font>(path);
            auto name = std::filesystem::path(path).stem().string();
            AddResource<Font>(name, uuid, font);
            return font;
        }
        TSO_CORE_WARN("Font {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->fontMap[uuid];
}

template<>Ref<AnimationClip>Resource::GetResource(const UUID& uuid){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->animationClipMap.find(uuid) == s_Resource->m_ResourceData->animationClipMap.end()){
        if(s_Resource->m_ResourceData->resourceUUIDPathMap.find(uuid) != s_Resource->m_ResourceData->resourceUUIDPathMap.end()){
            auto path = s_Resource->m_ResourceData->resourceUUIDPathMap[uuid];
            auto animation = CreateRef<AnimationClip>(path);
            auto name = std::filesystem::path(path).stem().string();
            AddResource<AnimationClip>(name, uuid, animation);
            return animation;
        }
        TSO_CORE_WARN("AnimationClip {} not found!" , uuid);
        return nullptr;
    }
    return s_Resource->m_ResourceData->animationClipMap[uuid];
}
//===============================================GetResource============================================================


//=============================================GetResourceMap===========================================================
template<>std::unordered_map<UUID , Ref<Shader>>& Resource::GetResourceMap(){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    return s_Resource->m_ResourceData->shaderMap;
}

template<>std::unordered_map<UUID , Ref<Material>>& Resource::GetResourceMap(){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    return s_Resource->m_ResourceData->materialMap;
}

template<>std::unordered_map<UUID , Ref<Texture2D>>& Resource::GetResourceMap(){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    return s_Resource->m_ResourceData->textureMap;
}

template<>std::unordered_map<UUID , Ref<Font>>& Resource::GetResourceMap(){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    return s_Resource->m_ResourceData->fontMap;
}

template<>std::unordered_map<UUID , Ref<AnimationClip>>& Resource::GetResourceMap(){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    return s_Resource->m_ResourceData->animationClipMap;
}

//=============================================GetResourceMap===========================================================

//===============================================AddResource============================================================
template<> void Resource::AddResource<Shader>(const std::string& name , const UUID &uuid, Ref<Shader> resource){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->shaderMap.find(uuid) != s_Resource->m_ResourceData->shaderMap.end()){
        TSO_CORE_WARN("Duplicated shader {} added in the asset, replacing..." , uuid);
    }
    s_Resource->m_ResourceData->shaderMap[uuid] = resource;
    resource->SetName(name);
    resource->SetUUID(uuid);
    s_Resource->m_ResourceData->resourcePathMap[resource->GetPath()] = uuid;
}

template<> void Resource::AddResource<Material>(const std::string& name , const UUID &uuid, Ref<Material> resource){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->materialMap.find(uuid) != s_Resource->m_ResourceData->materialMap.end()){
        TSO_CORE_WARN("Duplicated material {} added in the asset, replacing..." , uuid);
    }
    s_Resource->m_ResourceData->materialMap[uuid] = resource;
    resource->SetUUID(uuid);
    s_Resource->m_ResourceData->resourcePathMap[resource->GetPath()] = uuid;
}

template<> void Resource::AddResource<Texture2D>(const std::string& name , const UUID &uuid, Ref<Texture2D> resource){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->textureMap.find(uuid) != s_Resource->m_ResourceData->textureMap.end()){
        TSO_CORE_WARN("Duplicated texture {} added in the asset, replacing..." , uuid);
    }
    s_Resource->m_ResourceData->textureMap[uuid] = resource;
    resource->SetName(name);
    resource->SetUUID(uuid);
    s_Resource->m_ResourceData->resourcePathMap[resource->GetPath()] = uuid;
}

template<> void Resource::AddResource<Font>(const std::string& name , const UUID &uuid, Ref<Font> resource){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->fontMap.find(uuid) != s_Resource->m_ResourceData->fontMap.end()){
        TSO_CORE_WARN("Duplicated Font {} added in the asset, replacing..." , uuid);
    }
    s_Resource->m_ResourceData->fontMap[uuid] = resource;
    resource->SetName(name);
    resource->SetUUID(uuid);
    s_Resource->m_ResourceData->resourcePathMap[resource->GetPath()] = uuid;
}

template<> void Resource::AddResource<AnimationClip >(const std::string& name , const UUID &uuid, Ref<AnimationClip> resource){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->animationClipMap.find(uuid) != s_Resource->m_ResourceData->animationClipMap.end()){
        TSO_CORE_WARN("Duplicated Font {} added in the asset, replacing..." , uuid);
    }
    s_Resource->m_ResourceData->animationClipMap[uuid] = resource;
    resource->SetName(name);
    resource->SetUUID(uuid);
    s_Resource->m_ResourceData->resourcePathMap[resource->GetPath()] = uuid;
}

//===============================================AddResource============================================================

std::vector<std::filesystem::path> Resource::GetAllResourceToExport(){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    std::vector<std::filesystem::path>res;
    int resourceCount = GetResourceMap<Texture2D>().size() + GetResourceMap<Shader>().size() + GetResourceMap<Material>().size() + GetResourceMap<Font>().size() + GetResourceMap<AnimationClip>().size() + ScriptingEngine::GetScriptClasses().size();
    res.reserve(resourceCount * 2);
    for(auto& [uuid , texture] : GetResourceMap<Texture2D>()){
        res.push_back(texture->GetPath());
        res.push_back(GetMetaPath(texture->GetPath()));
    }
    for(auto& [uuid , shader] : GetResourceMap<Shader>()){
        res.push_back(shader->GetPath());
        res.push_back(GetMetaPath(shader->GetPath()));
    }
    for(auto& [uuid , mat] : GetResourceMap<Material>()){
        res.push_back(mat->GetPath());
        res.push_back(GetMetaPath(mat->GetPath()));
    }
    for(auto& [uuid , animation] : GetResourceMap<AnimationClip>()){
        res.push_back(animation->GetPath());
        res.push_back(GetMetaPath(animation->GetPath()));
    }
    for(auto& [uuid , font] : GetResourceMap<Font>()){
        res.push_back(font->GetPath());
        res.push_back(GetMetaPath(font->GetPath()));
    }
    for(auto& [name , script] : ScriptingEngine::GetScriptClasses()){
        res.push_back(script->GetPath());
        res.push_back(GetMetaPath(script->GetPath()));
    }
    return res;
}

UUID Resource::GetUUIDFromPath(const std::string &path){
    TSO_CORE_ASSERT(s_Resource != nullptr && s_Resource->m_ResourceData != nullptr , "didn't init resource yet");
    if(s_Resource->m_ResourceData->resourcePathMap.find(path) != s_Resource->m_ResourceData->resourcePathMap.end()){
        return s_Resource->m_ResourceData->resourcePathMap[path];
    }
    return 0;
}

    UUID Resource::ImportAsset(const std::filesystem::path& path) {
            // 1. 检查是否存在 .meta 文件
        std::filesystem::path metaPath = GetMetaPath(path);
        

        UUID metaUUID = ReadUUIDFromMeta(metaPath);
        if(metaUUID){
            return metaUUID;
        }

        // 2. 生成新 UUID
        UUID uuid; // 构造函数会自动生成随机 ID

        // 3. 确定资源类型 (可选，但推荐存入 meta)
        // 简单根据后缀判断
        std::string typeString = "Unknown";
        std::string ext = path.extension().string();
        if (ext == ".png" || ext == ".jpg") typeString = "Texture";
        else if (ext == ".glsl") typeString = "Shader";
        else if (ext == ".mat") typeString = "Material";
        else if (ext == ".anim") typeString = "Animation";
        else if (ext == ".lua") typeString = "Script";

        // 4. 创建并写入 .meta 文件
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "UUID" << YAML::Value << (uint64_t)uuid;
        out << YAML::Key << "Type" << YAML::Value << typeString;
        // 你可以在这里添加更多导入设置，比如 Texture 的 Filter Mode
        out << YAML::EndMap;

        std::ofstream fout(metaPath);
        fout << out.c_str();

            // 5. 注册到运行时系统 (内存映射)
            // 假设你有 s_AssetRegistry 或类似的 Map
        s_Resource->m_ResourceData->resourcePathMap[path] = uuid;
        s_Resource->m_ResourceData->resourceUUIDPathMap[uuid] = path;

            TSO_CORE_INFO("Imported Asset: {0} (UUID: {1})", path.filename().string(), (uint64_t)uuid);
            return uuid;
        }



}
