//
//  Material.cpp
//  TsoEngine
//
//  Created by 左斯诚 on 2025/10/29.
//
// Tso/Renderer/Material.cpp

#include "TPch.h"
#include "Material.h"
#include "RenderCommand.h"
#include "Tso/Project/ProjectSerielizer.h"
#include "Tso/Project/Resource.h"




namespace Tso {

    Material::Material(Ref<Shader> shader, const std::string& name)
        : m_Shader(shader), m_Name(name) {
        TSO_CORE_ASSERT(shader, "Shader is null!");
    }

Material::Material(const std::string& name)
    :m_Name(name) {
    //TSO_CORE_ASSERT(shader, "Shader is null!");
}

    Ref<Material> Material::Create(Ref<Shader> shader, const std::string& name) {
        return CreateRef<Material>(shader, name);
    }

    Ref<Material> Material::Create(const std::string& path , const UUID& uuid){
        return Desealize(path, uuid);
    }

    Ref<Material> Material::Create(const std::string& name) {
        return CreateRef<Material>(name);
    }


    void Material::Bind() const {
        if(!m_Shader){
            TSO_CORE_WARN("No shader in Material {}" , m_Name);
            return;
        }
        // 1. 绑定Shader程序
        m_Shader->Bind();

        // 2. 上传所有基础类型的Uniforms
        for (const auto& [name, value] : m_FloatParams) {
            m_Shader->SetFloat(name, value);
        }
        for (const auto& [name, value] : m_IntParams) {
            m_Shader->SetInt(name, value);
        }
        for (const auto& [name, value] : m_Vec2Params) {
            m_Shader->SetFloat2(name, value);
        }
        for (const auto& [name, value] : m_Vec3Params) {
            m_Shader->SetFloat3(name, value);
        }
        for (const auto& [name, value] : m_Vec4Params) {
            m_Shader->SetFloat4(name, value);
        }
        for (const auto& [name, value] : m_Mat3Params) {
            m_Shader->SetMatrix3(name, value);
        }
        for (const auto& [name, value] : m_Mat4Params) {
            m_Shader->SetMatrix4(name, value);
        }

        // 3. 绑定所有纹理，并设置对应的Sampler Uniform
        uint32_t textureSlot = 0;
        for (const auto& [name, texture] : m_TextureParams) {
            if (texture) {
                // 将纹理绑定到下一个可用的纹理单元(slot)
                texture->Bind(textureSlot);
                // 告诉Shader，名为 'name' 的sampler2D应该从这个slot读取数据
                m_Shader->SetInt(name, textureSlot);
                textureSlot++;
            }
        }
    }

    void Material::Unbind() const {
        m_Shader->UnBind();
    }
    
    // --- Setters的实现 ---
    // 非常直接，就是把值存入对应的map中

    void Material::SetFloat(const std::string& name, float value) {
        m_FloatParams[name] = value;
    }

    void Material::SetInt(const std::string& name, int value) {
        m_IntParams[name] = value;
    }

    void Material::SetVec2(const std::string& name, const glm::vec2& value) {
        m_Vec2Params[name] = value;
    }

    void Material::SetVec3(const std::string& name, const glm::vec3& value) {
        m_Vec3Params[name] = value;
    }

    void Material::SetVec4(const std::string& name, const glm::vec4& value) {
        m_Vec4Params[name] = value;
    }

    void Material::SetMat3(const std::string& name, const glm::mat3& value) {
        m_Mat3Params[name] = value;
    }

    void Material::SetMat4(const std::string& name, const glm::mat4& value) {
        m_Mat4Params[name] = value;
    }

    void Material::SetTexture(const std::string& name, const Ref<Texture2D>& texture) {
        m_TextureParams[name] = texture;
    }

    void Material::DefineInstanceParam(const std::string& name, int slotIndex) {
        if (slotIndex < 0 || slotIndex > 3) {
            TSO_CORE_WARN("Slot index must be 0-3");
            return;
        }
        m_InstanceParamMap[name] = slotIndex;
    }

    int Material::GetInstanceParamSlot(const std::string& name) const {
        if (m_InstanceParamMap.find(name) != m_InstanceParamMap.end())
            return m_InstanceParamMap.at(name);
        return -1;
    }

    void Material::Serealize() {
        if (m_filePath.empty()) {
            TSO_CORE_ERROR("Cannot save material [{0}] to an empty path", m_Name);
            return;
        }

        YAML::Emitter out;
        {
            out << YAML::BeginMap; // Root
            out << YAML::Key << "Name" << YAML::Value << m_Name;

            out << YAML::Key << "Variant" << YAML::Value << YAML::BeginSeq; // Variant List

            // 1. 序列化 Shader
            // 加载代码中依赖 Type="Shader" 来创建材质，所以必须写进去
            if (m_Shader) {
                out << YAML::BeginMap;
                out << YAML::Key << "Name" << YAML::Value << "u_Shader"; // 名字通常不重要，但需要占位
                out << YAML::Key << "Type" << YAML::Value << "Shader";
                // 注意：这里需要 Shader 的 Asset UUID，而不是 OpenGL Program ID
                // 请根据你的 Resource 系统替换 GetRendererID()
                out << YAML::Key << "Value" << YAML::Value << (uint64_t)m_Shader->GetUUID();
                out << YAML::EndMap;
            }

            // 2. 序列化 Textures
            for (auto& [name, texture] : m_TextureParams) {
                if (texture) {
                    out << YAML::BeginMap;
                    out << YAML::Key << "Name" << YAML::Value << name;
                    out << YAML::Key << "Type" << YAML::Value << "Texture";
                    // 同样，需要 Texture 的 Asset UUID
                    out << YAML::Key << "Value" << YAML::Value << (uint64_t)texture->GetUUID();
                    out << YAML::EndMap;
                }
            }

            // 3. 序列化 Ints
            for (auto& [name, val] : m_IntParams) {
                out << YAML::BeginMap;
                out << YAML::Key << "Name" << YAML::Value << name;
                out << YAML::Key << "Type" << YAML::Value << "Int";
                out << YAML::Key << "Value" << YAML::Value << val;
                out << YAML::EndMap;
            }

            // 4. 序列化 Floats
            for (auto& [name, val] : m_FloatParams) {
                out << YAML::BeginMap;
                out << YAML::Key << "Name" << YAML::Value << name;
                out << YAML::Key << "Type" << YAML::Value << "Float";
                out << YAML::Key << "Value" << YAML::Value << val;
                out << YAML::EndMap;
            }

            // 5. 序列化 Vec2
            for (auto& [name, val] : m_Vec2Params) {
                out << YAML::BeginMap;
                out << YAML::Key << "Name" << YAML::Value << name;
                out << YAML::Key << "Type" << YAML::Value << "Vec2";
                out << YAML::Key << "Value" << YAML::Value << val; // 使用了上面的重载
                out << YAML::EndMap;
            }

            // 6. 序列化 Vec3
            for (auto& [name, val] : m_Vec3Params) {
                out << YAML::BeginMap;
                out << YAML::Key << "Name" << YAML::Value << name;
                out << YAML::Key << "Type" << YAML::Value << "Vec3";
                out << YAML::Key << "Value" << YAML::Value << val;
                out << YAML::EndMap;
            }

            // 7. 序列化 Vec4
            for (auto& [name, val] : m_Vec4Params) {
                out << YAML::BeginMap;
                out << YAML::Key << "Name" << YAML::Value << name;
                out << YAML::Key << "Type" << YAML::Value << "Vec4";
                out << YAML::Key << "Value" << YAML::Value << val;
                out << YAML::EndMap;
            }
            
            // 8. 序列化 Matrices (Mat3 / Mat4)
            for (auto& [name, val] : m_Mat3Params) {
                out << YAML::BeginMap;
                out << YAML::Key << "Name" << YAML::Value << name;
                out << YAML::Key << "Type" << YAML::Value << "Mat3";
                out << YAML::Key << "Value" << YAML::Value << val;
                out << YAML::EndMap;
            }

            for (auto& [name, val] : m_Mat4Params) {
                out << YAML::BeginMap;
                out << YAML::Key << "Name" << YAML::Value << name;
                out << YAML::Key << "Type" << YAML::Value << "Mat4";
                out << YAML::Key << "Value" << YAML::Value << val;
                out << YAML::EndMap;
            }

            out << YAML::EndSeq; // Variant
            out << YAML::EndMap; // Root
        }

        std::ofstream fout(m_filePath);
        fout << out.c_str();
        
        TSO_CORE_INFO("Material saved successfully to: {0}", m_filePath);
    }

    Ref<Material> Material::Desealize(const std::string& path , const UUID& uuid){
        Tso::Buffer fileBuffer = Tso::VirtualFileSystem::ReadFile(path);
        if (!fileBuffer.IsValid()) {
            TSO_CORE_ASSERT(false, "Failed to load file from VFS: {0}", path);
            return nullptr;
        }

        YAML::Node data;
        try {
            std::string yamlString(fileBuffer.DataPtr(), fileBuffer.Size());
            data = YAML::Load(yamlString);
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
        Ref<Shader> shader = nullptr;
        for (auto var : variantNode) {
            auto type = var["Type"].as<std::string>();
            if (type == "Shader") {
                UUID shaderUUID = var["Value"].as<uint64_t>();
                shader = Resource::GetResource<Shader>(shaderUUID);
                break;
            }
        }

        if (!shader) {
            TSO_CORE_ERROR("Material '{0}' does not specify a Shader or Shader asset not found!", materialName);
            return nullptr;
        }

        // --- 第二步：创建 Material 实例 ---
        Ref<Material> material = Material::Create(shader, materialName);
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
                UUID textureUUID = valueNode.as<uint64_t>();
                Ref<Texture2D> texture = Resource::GetResource<Texture2D>(textureUUID);
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


}
