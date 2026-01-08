#include "TPch.h"
#include "ProjectSerielizer.h"
#include "yaml-cpp/yaml.h"
#include <fstream>
#include "Tso/Core/UUID.h"
#include "Tso/Project/Resource.h"
#include "Tso/Renderer/Material.h"

namespace YAML {

    template<>
    struct convert<glm::vec2> {
        static bool decode(const Node& node, glm::vec2& rhs) {
            if (!node.IsSequence() || node.size() != 2) return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec3> {
        static bool decode(const Node& node, glm::vec3& rhs) {
            if (!node.IsSequence() || node.size() != 3) return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4> {
        static bool decode(const Node& node, glm::vec4& rhs) {
            if (!node.IsSequence() || node.size() != 4) return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };
    
    // 简单的 Mat3/Mat4 解析 (假设 YAML 中存储为扁平数组)
    // 如果你的 mat4 存储为嵌套数组，逻辑需要调整
    template<>
    struct convert<glm::mat3> {
        static bool decode(const Node& node, glm::mat3& rhs) {
            if (!node.IsSequence() || node.size() != 9) return false;
            // glm::mat3 是列主序，这里假设 YAML 也是列主序或者你需要转置
            for (int i = 0; i < 9; i++) rhs[i / 3][i % 3] = node[i].as<float>();
            return true;
        }
    };
    
    template<>
    struct convert<glm::mat4> {
        static bool decode(const Node& node, glm::mat4& rhs) {
            if (!node.IsSequence() || node.size() != 16) return false;
            for (int i = 0; i < 16; i++) rhs[i / 4][i % 4] = node[i].as<float>();
            return true;
        }
    };
}

namespace Utils{
    void LoadProjectAsset(const YAML::Node& node){
        auto resources = node;
        for(auto res : resources){
            auto type = res["Type"].as<std::string>();
            auto uuid = res["UUID"].as<uint64_t>();
            auto path = res["Path"].as<std::string>();
            auto name = res["Name"].as<std::string>();
            auto fullPath = Tso::Project::GetProjectDirectory() / std::filesystem::path(path);
            if(!std::filesystem::exists(fullPath)){
                TSO_CORE_ASSERT(false , "Resource [{}] not found!" , fullPath.string());
            }
            if(type == "Shader"){
                Tso::Ref<Tso::Shader> shader = Tso::Shader::Create(fullPath.string());
                shader->SetName(name);
                shader->SetPath(fullPath);
                if(shader){
                    Tso::Resource::AddResource(name, uuid, shader);
                    TSO_CORE_TRACE("Add Shader {} successfully", uuid);
                }
                else{
                    TSO_CORE_WARN("Shader {} fails to be generated!", uuid);
                }
            }
            else if(type == "Texture"){
                
                auto texture = Tso::Texture2D::Create(fullPath.string());
                texture->SetName(name);
                if(texture){
                    Tso::Resource::AddResource(name , uuid, texture);
                    TSO_CORE_TRACE("Add Texture {} successfully", uuid);
                }
                else{
                    TSO_CORE_WARN("Texture {} fails to be generated!", uuid);
                }
            }
            else{
                TSO_CORE_ASSERT(false , "Unknow type of resource: [{}]" ,type);
            }
        }
    }

    Tso::Ref<Tso::Material> LoadMaterial(const Tso::UUID& uuid, std::string& path) {
            YAML::Node data;
            try {
                data = YAML::LoadFile(path);
            }
            catch (YAML::ParserException e) {
                TSO_CORE_ERROR("Failed to load Material file '{0}'\n     {1}", path, e.what());
                return nullptr;
            }

            // 假设 YAML 根节点有个 Name 属性，或者我们用路径名
            std::string materialName = data["Name"] ? data["Name"].as<std::string>() : "Unnamed Material";
            auto variantNode = data["Variant"];
            if (!variantNode) return nullptr;

            // --- 第一步：寻找 Shader ---
            // Material 的构造函数依赖 Shader，所以必须先找到 Shader 的 UUID
            Tso::Ref<Tso::Shader> shader = nullptr;
            for (auto var : variantNode) {
                auto type = var["Type"].as<std::string>();
                if (type == "Shader") {
                    Tso::UUID shaderUUID = var["Value"].as<uint64_t>();
                    // 通过 AssetManager 获取 Shader 实例
                    shader = Tso::Resource::GetShader(shaderUUID);
                    break;
                }
            }

            if (!shader) {
                TSO_CORE_ERROR("Material '{0}' does not specify a Shader or Shader asset not found!", materialName);
                // 这里可以返回一个默认的“紫色错误材质”，或者直接返回空
                return nullptr;
            }

            // --- 第二步：创建 Material 实例 ---
            Tso::Ref<Tso::Material> material = Tso::Material::Create(shader, materialName);
            material->SetRendererID(uuid);
            material->SetPath(path);
            // --- 第三步：填充 Uniforms 和 Textures ---
            for (auto var : variantNode) {
                auto name = var["Name"].as<std::string>(); // 必须要有参数名
                auto type = var["Type"].as<std::string>();
                auto valueNode = var["Value"];

                // 你的 Material 类中对应的方法
                if (type == "Shader") {
                    // 已经处理过了，跳过
                    continue;
                }
                else if (type == "Texture") {
                    Tso::UUID textureUUID = valueNode.as<uint64_t>();
                    Tso::Ref<Tso::Texture2D> texture = Tso::Resource::GetTexture(textureUUID);
                    if (texture) {
                        material->SetTexture(name, texture);
                    } else {
                        TSO_CORE_WARN("Material '{0}': Texture asset {1} not found for '{2}'", materialName, textureUUID, name);
                    }
                }
                else if (type == "Int") {
                    material->SetInt(name, valueNode.as<int>());
                }
                else if (type == "Float") {
                    material->SetFloat(name, valueNode.as<float>());
                }
                else if (type == "Vec2") {
                    material->SetVec2(name, valueNode.as<glm::vec2>());
                }
                else if (type == "Vec3") {
                    material->SetVec3(name, valueNode.as<glm::vec3>());
                }
                else if (type == "Vec4") {
                    material->SetVec4(name, valueNode.as<glm::vec4>());
                }
                else if (type == "Mat3") {
                    material->SetMat3(name, valueNode.as<glm::mat3>());
                }
                else if (type == "Mat4") {
                    material->SetMat4(name, valueNode.as<glm::mat4>());
                }
                else {
                    TSO_CORE_WARN("Material '{0}': Unknown uniform type '{1}' for '{2}'", materialName, type, name);
                }
            }

            return material;
        }
    void LoadMaterials(const YAML::Node& node){
        auto matNode = node;
        for(auto mat : matNode){
            if(!mat["Path"] || !mat["UUID"]){
                TSO_CORE_ERROR("Missing key Path or UUID in material deseralizing!");
            }
            std::string path = mat["Path"].as<std::string>();
            Tso::UUID uuid = mat["UUID"].as<uint64_t>();
            auto fullPath = (Tso::Project::GetProjectDirectory() / std::filesystem::path(path)).string();
            auto material = LoadMaterial(uuid, fullPath);
            if(material){
                Tso::Resource::AddResource("" , uuid, material);
                TSO_CORE_TRACE("Add material {} successfully", uuid);
            }
            else{
                TSO_CORE_WARN("Failed to create material {}.",uuid);
            }
        }
    }
}


namespace Tso {
	ProjectSerielizer::ProjectSerielizer(Ref<Project> project)
		: m_Project(project)
	{
	}

    bool ProjectSerielizer::Serielize(const std::string& prjPath)
    {
        const auto& config = m_Project->GetConfig();
        
        // 获取项目所在的根目录，用于计算相对路径
        std::filesystem::path projectDir = std::filesystem::path(Project::GetProjectDirectory());

        YAML::Emitter out;
        {
            out << YAML::BeginMap; // Root Map
            out << YAML::Key << "Project" << YAML::Value;
            {
                out << YAML::BeginMap; // Project Map
                
                // --- 1. 基础配置 ---
                out << YAML::Key << "Name" << YAML::Value << config.Name;
                out << YAML::Key << "StartScene" << YAML::Value << config.FirstScene.string();
                out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetPath.string();
                out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptModulePath.string();

                // --- 2. Resource 列表 (Texture & Shader) ---
                out << YAML::Key << "Resource" << YAML::Value << YAML::BeginSeq;

                // 2.1 序列化 Textures
                auto& textures = Resource::GetAllTextures();
                for (auto& [uuid, texture] : textures)
                {
                    out << YAML::BeginMap;
                    out << YAML::Key << "Type" << YAML::Value << "Texture";
                    out << YAML::Key << "Name" << YAML::Value << texture->GetName();
                    out << YAML::Key << "UUID" << YAML::Value << (uint64_t)uuid; // 确保存储为数字
                    
                    // 计算相对路径： assets/sprites.png
                    std::filesystem::path absPath = texture->GetPath();
                    std::string relPath = std::filesystem::relative(absPath, projectDir).generic_string(); // generic_string 确保使用 / 分隔符
                    out << YAML::Key << "Path" << YAML::Value << relPath;
                    
                    out << YAML::EndMap;
                }

                // 2.2 序列化 Shaders
                auto& shaders = Resource::GetAllShaders();
                for (auto& [uuid, shader] : shaders)
                {
                    out << YAML::BeginMap;
                    out << YAML::Key << "Type" << YAML::Value << "Shader";
                    out << YAML::Key << "Name" << YAML::Value << shader->GetName();
                    out << YAML::Key << "UUID" << YAML::Value << (uint64_t)uuid;

                    // 假设 Shader 类有 GetFilePath()
                    std::filesystem::path absPath = shader->GetPath();
                    std::string relPath = std::filesystem::relative(absPath, projectDir).generic_string();
                    out << YAML::Key << "Path" << YAML::Value << relPath;

                    out << YAML::EndMap;
                }

                out << YAML::EndSeq; // End Resource List


                // --- 3. Materials 列表 ---
                // 注意：材质的具体参数保存在 .mat 文件中，这里只保存项目引用了哪些材质文件
                out << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;

                auto& materials = Resource::GetAllMaterials();
                for (auto& [uuid, material] : materials)
                {
                    // 只有当材质有对应的物理文件路径时才保存到 Project 中
                    // 内存中临时创建的材质如果不保存为 .mat，这里可能没法记录路径
                    // 假设 Material 类有 GetPath() 成员
                    std::string matPathStr = material->GetPath();
                    if (matPathStr.empty()) continue;
                    material->Serealize();
                    out << YAML::BeginMap;
                    out << YAML::Key << "UUID" << YAML::Value << (uint64_t)uuid;

                    std::filesystem::path absPath = matPathStr;
                    std::string relPath = std::filesystem::relative(absPath, projectDir).generic_string();
                    out << YAML::Key << "Path" << YAML::Value << relPath;
                    
                    out << YAML::EndMap;
                    
                    // 顺便：通常在这里我们也应该调用 MaterialSerializer::Serialize(material, absPath);
                    // 确保材质本身的参数（颜色、贴图引用）也被保存到 .mat 文件中
                }

                out << YAML::EndSeq; // End Materials List

                out << YAML::EndMap; // End Project Map
            }
            out << YAML::EndMap; // End Root Map
        }
        auto prjFilePath = prjPath + "/" + config.Name + ".tproj";
        std::ofstream fout(prjFilePath);
        TSO_CORE_INFO("Project file saved to {} successfully!" , prjFilePath);
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
		config.FirstScene = std::filesystem::path(projectNode["StartScene"].as<std::string>());
		config.AssetPath = std::filesystem::path(projectNode["AssetDirectory"].as<std::string>());//not used
		config.ScriptModulePath = std::filesystem::path(projectNode["ScriptModulePath"].as<std::string>());
        if(auto resourceNode = projectNode["Resource"]){
            Utils::LoadProjectAsset(resourceNode);
        }
        if(auto matNode = projectNode["Materials"]){
            Utils::LoadMaterials(matNode);
        }
		return true;
	}
}
