#pragma once
#include "Tso/Renderer/Material.h"
#include "Tso/Renderer/Texture.h"
#include "Tso/Renderer/SubTexture2D.h"
#include "Tso/Renderer/Camera.h"
#include "glm/glm.hpp"
#include <vector>

namespace Tso {
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
        // ... 你的文字命令定义
    };

    class Renderer2DMaterial {
    public:
        struct Statistics {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;
            // ...
        };

        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCamera& camera);
        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void EndScene();
        
        static void Flush();

        // 提交命令
        static void DrawQuad(const glm::mat4& transform,const std::vector<glm::vec2>& texcoord , int entityID, Ref<Material> material,MaterialInstanceComponent* overrideComp);
//        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID, Ref<Material> material);
//        static void DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, int entityID, Ref<Material> material);
//        static void DrawQuad(const glm::mat4& transform, Ref<SubTexture2D> subTexture, int entityID, Ref<Material> material);
        

        static Statistics GetStats();
        static void ResetStats();

    private:
        static void StartBatch();
        static void RenderBatch(Ref<Material> material, Ref<Texture2D> overrideTexture, uint32_t count);
    };
}
