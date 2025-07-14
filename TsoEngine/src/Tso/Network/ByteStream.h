#pragma once
#include <string>
namespace Tso {
    class Entity;
    class Scene;
    
    class ByteStream {
    public:
        struct Header
        {
            uint8_t protocol = 0;
            size_t dataLength = 0;
        };
        static constexpr size_t HEADER_SIZE = sizeof(uint8_t) + sizeof(size_t);
    public:
        // 写入基本类型
        template<typename T>
        void write(const T& value);
        //
        template<typename T>
        void writeFront(const T& value);

        // 写入字符串
        void writeString(const std::string& str);

        // 读取基本类型
        template<typename T>
        T read() {
            static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T>,
                "Only arithmetic types and enums allowed");
            if (pos + sizeof(T) > buffer.size()) {
                throw std::runtime_error("Read out of bounds");
            }
            T value;
            memcpy(&value, buffer.data() + pos, sizeof(T));
            pos += sizeof(T);
            return value;
        }

        // 读取字符串
        std::string readString();

        ByteStream static SeriealizeEntity(Entity& entity, const uint8_t protocolID);
        void DeseriealizeEntity(Ref<Scene> scene);

        void static PackHeader(ByteStream& byte, const Header& header);

        ByteStream(std::vector<uint8_t> buffer);
        ByteStream() = default;

        const std::vector<uint8_t>& getBuffer() const { return buffer; }
        const void* getRawBuffer() { return static_cast<void*>(buffer.data()); }
        const void ResetRead() { pos = 0; }
        size_t getRawBufferLength()& { return buffer.size(); }
        void setBuffer(const std::vector<uint8_t>& newBuffer);
        bool operator==(ByteStream& other);
    private:
        std::vector<uint8_t> buffer;
        size_t pos = 0;
    };
}
