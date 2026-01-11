#pragma once
#include "Tso/Renderer/Material.h"
#include "Tso/Renderer/Texture.h"
#include "Tso/Renderer/SubTexture2D.h"
#include "Tso/Renderer/Camera.h"
#include "glm/glm.hpp"
#include <vector>
#include "Tso/Scene/Component.h"

namespace Tso {
class Font;

struct MaterialInstanceComponent;
    // 命令结构体
    struct DrawQuadCommand {
        glm::mat4 Transform;
        glm::vec4 Color;
        std::vector<glm::vec2> Texcoord;
        int EntityID;
        Ref<Material> Material;
        glm::vec4 custumData;
    };

    struct DrawStringCommand {
        Ref<Font> Font;
        glm::mat4 Transform;
        std::string Text;
        glm::vec4 Color;
        TextParam TextParam;
        int EntityID;
        Ref<Material> Material;
        glm::vec4 CustumData;
    };

    class Renderer2DMaterial {
    public:
        struct Statistics {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;
            uint32_t TextCount = 0;
            // ...
        };

        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCamera& camera);
        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void EndScene();
        
        static void Flush();

        // 提交命令
        static void DrawQuad(const glm::mat4& transform,const std::vector<glm::vec2>& texcoord,const glm::vec4& color , int entityID, Ref<Material> material,MaterialInstanceComponent* overrideComp);
        
        static void DrawString(const Ref<Font> font , const glm::mat4& transform , const std::string& text , const TextParam& textParam , const int& entityID, Ref<Material> material,MaterialInstanceComponent* overrideComp);


//        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID, Ref<Material> material);
//        static void DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, int entityID, Ref<Material> material);
//        static void DrawQuad(const glm::mat4& transform, Ref<SubTexture2D> subTexture, int entityID, Ref<Material> material);
        

        static Statistics GetStats();
        static void ResetStats();

    private:
        template<typename T>
        static void InitRenderer(Ref<VertexArray>& vertexArray, Ref<VertexBuffer>& vertexBuffer, T** vertexBase , Ref<IndexBuffer>& indexBuffer ,const BufferLayout& layout);
        static void StartBatch();
        static void RenderBatch(Ref<Material> material, Ref<Texture2D> overrideTexture, uint32_t count);
        static void RenderQuadBatch(Ref<Material> material, uint32_t count);
        static void RenderTextBatch(Ref<Material> material, uint32_t count);
        static uint32_t GenerateTextVertices(const DrawStringCommand& cmd);

        static void FlushQuad();
        static void FlushString();
    };
}
