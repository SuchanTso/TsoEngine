#include "TPch.h"
#include "ByteStream.h"
#include "Tso/Scene/Scene.h"
#include "Tso/Scene/Component.h"
namespace Tso {

    ByteStream ByteStream::SeriealizeEntity(Entity& entity, const uint8_t protocolID) {
        //for network 
        ByteStream entityByte;

        /*entityByte.write(protocolID);*/

        if (entity.HasComponent<TagComponent>())
        {
            entityByte.write(ComponentID::TagComponent);
            entityByte.writeString(entity.GetComponent<TagComponent>().GetTagName());
        }

        if (entity.HasComponent<ScriptComponent>())
        {
            //TODO(Suchan): script maybe not necessary
        }

        if (entity.HasComponent<TransformComponent>())
        {
            entityByte.write(ComponentID::TransformComponent);
            for (int i = 0; i < 3; i++) entityByte.write(entity.GetComponent<TransformComponent>().m_Translation[i]);
            for (int i = 0; i < 3; i++) entityByte.write(entity.GetComponent<TransformComponent>().m_Scale[i]);
            for (int i = 0; i < 3; i++) entityByte.write(entity.GetComponent<TransformComponent>().m_Rotation[i]);
        }

        if (entity.HasComponent<CameraComponent>())
        {
            //TODO(Suchan): camera maybe does not sync maybe
        }

        if (entity.HasComponent<Renderable>())
        {
            //TODO(Suchan):no need for network
        }


        if (entity.HasComponent<Rigidbody2DComponent>())
        {
            //TODO(Suchan):no need for network
        }

        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            //TODO(Suchan):no need for network
        }

        if (entity.HasComponent<TextComponent>())
        {
            entityByte.write(ComponentID::TextComponent);
            entityByte.writeString(entity.GetComponent<TextComponent>().Text);
        }

        if (entity.HasComponent<IDComponent>())
        {
            entityByte.write(ComponentID::IDComponent);
            entityByte.write((uint64_t)(entity.GetComponent<IDComponent>().ID));
        }

        PackHeader(entityByte, { protocolID , entityByte.getRawBufferLength() + ByteStream::HEADER_SIZE });
        return entityByte;
    }

    void ByteStream::DeseriealizeEntity(Ref<Scene> scene)
    {
        glm::vec3 translate;
        glm::vec3 scale;
        glm::vec3 rotation;
        std::string tag;
        std::string text;
        UUID uuid;
        bool transformTag = false;

        while (pos < buffer.size()) {
            ComponentID CompID = (ComponentID)read<uint16_t>();
            if (CompID == ComponentID::TagComponent) {
                tag = readString();
            }
            else if (CompID == ComponentID::TransformComponent) {
                for (int i = 0; i < 3; i++) translate[i] = read<float>();
                for (int i = 0; i < 3; i++) scale[i] = read<float>();
                for (int i = 0; i < 3; i++) rotation[i] = read<float>();
                transformTag = true;
            }
            else if (CompID == ComponentID::TextComponent) {
                text = readString();
            }
            else if (CompID == ComponentID::IDComponent) {
                uuid = read<uint64_t>();
            }
            else {
                TSO_CORE_ERROR("unknown componentID:{}", (uint16_t)CompID);
                return;
            }
        }
        if (scene->HasEntity(uuid)) {
            auto entity = scene->GetEntityByUUID(uuid);
            if (transformTag) {
                auto& transform = entity.GetComponent<TransformComponent>();
                transform.m_Translation = translate;
                transform.m_Scale = scale;
                transform.m_Rotation = rotation;
            }
            if (!tag.empty()) {
                auto& tagcomp = entity.GetComponent<TagComponent>();
                tagcomp.m_Name = tag;
            }
            if (!text.empty()) {
                auto& textcomp = entity.GetComponent<TextComponent>();
                textcomp.Text = text;
            }
        }
        else {
            // didn't find the entity , create one
            auto entity = scene->CreateEntityWithID(uuid);
            if (transformTag) {
                auto& transform = entity.GetComponent<TransformComponent>();
                transform.m_Translation = translate;
                transform.m_Scale = scale;
                transform.m_Rotation = rotation;
            }
            if (!tag.empty()) {
                auto& tagcomp = entity.GetComponent<TagComponent>();
                tagcomp.m_Name = tag;
            }
            if (!text.empty()) {
                auto& textcomp = entity.GetComponent<TextComponent>();
                textcomp.Text = text;
            }
        }
    }

    void ByteStream::PackHeader(ByteStream& byte, const ByteStream::Header& header)
    {
        byte.writeFront(header.dataLength);
        byte.writeFront(header.protocol);
    }

    ByteStream::ByteStream(std::vector<uint8_t> buffer) : buffer(buffer)
    {
    }


    template<typename T>
    void ByteStream::write(const T& value) {
        TSO_CORE_ASSERT(std::is_arithmetic_v<T> || std::is_enum_v<T>,
            "Only arithmetic types and enums allowed");
        const char* data = reinterpret_cast<const char*>(&value);
        buffer.insert(buffer.end(), data, data + sizeof(T));
    }

    template<typename T>
    void ByteStream::writeFront(const T& value) {
        TSO_CORE_ASSERT(std::is_arithmetic_v<T> || std::is_enum_v<T>,
            "Only arithmetic types and enums allowed");
        const char* data = reinterpret_cast<const char*>(&value);
        buffer.insert(buffer.begin(), data, data + sizeof(T));
    }

    // 写入字符串
    void ByteStream::writeString(const std::string& str) {
        write<uint32_t>(static_cast<uint32_t>(str.size()));
        buffer.insert(buffer.end(), str.begin(), str.end());
    }

    // 读取基本类型
   

    // 读取字符串
    std::string ByteStream::readString() {
        uint32_t len = read<uint32_t>();
        if (pos + len > buffer.size()) {
            throw std::runtime_error("Read out of bounds");
        }
        std::string str(buffer.begin() + pos, buffer.begin() + pos + len);
        pos += len;
        return str;
    }

    //const std::vector<uint8_t>& ByteStream::getBuffer() const { return buffer; }

    void ByteStream::setBuffer(const std::vector<uint8_t>& newBuffer) {
        buffer = newBuffer;
        pos = 0;
    }

    bool ByteStream::operator==(ByteStream& other) {
        if (buffer.size() != other.buffer.size()) return false;
        for (int i = 0; i < buffer.size(); i++)
            if (buffer[i] != other.buffer[i]) return false;
        return true;
    }
}