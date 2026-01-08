//
//  Renderer2DMaterial.cpp
//  TsoEngine
//
//  Created by SuchanTso
//

#include "TPch.h"
#include "Renderer2DMaterial.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/OrthographicCamera.h"
#include "Material.h"

namespace Tso {

    // --- 内部数据结构定义 ---

    struct QuadVertex {
        glm::vec3 Position;
        glm::vec2 TexCoord;
        glm::vec4 Color;
        float textureIndex;
        float EntityID;
    };

    struct Renderer2DMaterialData {
        // 最大批处理限制
        static const uint32_t MaxQuads = 2000;
        static const uint32_t MaxVertices = MaxQuads * 4;
        static const uint32_t MaxIndices = MaxQuads * 6;

        Ref<VertexArray> QuadVAO;
        Ref<VertexBuffer> QuadVBO;
        
        Ref<VertexArray> TextVAO;
        Ref<VertexBuffer> TextVBO;

        // CPU侧的顶点缓冲区指针（用于构建批次数据）
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;
        
        // 统计数据
        Renderer2DMaterial::Statistics Stats;

        // 场景状态
        glm::mat4 ViewProjection;
        bool SceneBegun = false;

        // 命令队列
        std::vector<DrawQuadCommand> QuadCommands;
        std::vector<DrawStringCommand> StringCommands; // 暂时保留，逻辑类似Quad

        Renderer2DMaterialData() {
            QuadVertexBufferBase = new QuadVertex[MaxVertices];
        }

        ~Renderer2DMaterialData() {
            delete[] QuadVertexBufferBase;
        }
    };

    static Renderer2DMaterialData s_Data;

    // --- 初始化与销毁 ---

    void Renderer2DMaterial::Init() {
        s_Data.QuadCommands.reserve(1024);
        
        // 1. 设置 Quad VAO/VBO/IBO
        s_Data.QuadVAO = VertexArray::Create();

        s_Data.QuadVBO = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
        s_Data.QuadVBO->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float2, "a_TexCoord" },
            { ShaderDataType::Float4, "a_Color" },
            {ShaderDataType::Float  , "a_TexIndex"},
            { ShaderDataType::Float, "a_EntityID" } // int在shader中读取建议用float传或专门的IVertexAttrib
        });
        s_Data.QuadVAO->AddVertexBuffer(s_Data.QuadVBO);

        // 创建索引缓冲区 (预计算)
        uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6) {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }
        Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
        s_Data.QuadVAO->SetIndexBuffer(quadIB);
        delete[] quadIndices;

        // 2. 初始化 Text VAO (略，逻辑与Quad类似)
        // ...
    }

    void Renderer2DMaterial::Shutdown() {
        // 智能指针会自动释放 OpenGL 资源，这里只需清理 raw pointers
        // s_Data 析构函数会处理 delete[]
    }

    // --- 场景控制 ---

    void Renderer2DMaterial::BeginScene(const OrthographicCamera& camera) {
//        s_Data.ViewProjection = camera.GetViewProjectionMatrix();
//        StartBatch();
//        s_Data.SceneBegun = true;
    }

    void Renderer2DMaterial::BeginScene(const Camera& camera, const glm::mat4& transform) {
        s_Data.ViewProjection = camera.GetProjection() * glm::inverse(transform);
        StartBatch();
        s_Data.SceneBegun = true;
    }

    void Renderer2DMaterial::EndScene() {
        Flush();
        s_Data.SceneBegun = false;
    }

    // --- 核心渲染逻辑 ---

    void Renderer2DMaterial::StartBatch() {
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
        s_Data.QuadCommands.clear();
        s_Data.Stats = Statistics();
    }

    void Renderer2DMaterial::Flush() {
        if (s_Data.QuadCommands.empty())
            return;

        // 1. 排序：按材质指针排序，尽可能让相同材质的DrawCall靠在一起
        // 如果材质相同，再按纹理排序（针对DrawQuad中传入了特定纹理的情况）
        std::sort(s_Data.QuadCommands.begin(), s_Data.QuadCommands.end(),
            [](const DrawQuadCommand& a, const DrawQuadCommand& b) {
//                if (a.Material != b.Material)
                return a.Material < b.Material;
//                void* texA = a.SubTexture ? (void*)a.SubTexture->GetTexture().get() : (void*)a.Texture.get();
//                void* texB = b.SubTexture ? (void*)b.SubTexture->GetTexture().get() : (void*)b.Texture.get();
//                return texA < texB;
            });

        // 2. 遍历命令并构建批次
        // 当前批次的状态
        Ref<Material> currentMaterial = nullptr;
        Ref<Texture2D> currentOverrideTexture = nullptr;
        uint32_t quadCount = 0;

        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        for (const auto& cmd : s_Data.QuadCommands) {
            if (!cmd.Material) continue; // 忽略无效材质

            // 解析当前命令需要的纹理（如果有）
            Ref<Texture2D> cmdTexture = nullptr;
//            if (cmd.SubTexture) cmdTexture = cmd.SubTexture->GetTexture();
//            else if (cmd.Texture) cmdTexture = cmd.Texture;

            // 检查是否需要开始新批次 (DrawCall)
            // 条件：材质改变 OR 覆盖纹理改变 OR 缓冲区已满
            bool materialChanged = (currentMaterial != cmd.Material);
            bool bufferFull = (quadCount >= Renderer2DMaterialData::MaxQuads);

            if (quadCount > 0 && (materialChanged || bufferFull)) {
                RenderBatch(currentMaterial, currentOverrideTexture, quadCount);
                
                // 重置缓冲区指针和计数
                quadCount = 0;
                s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
            }

            // 更新当前状态
            currentMaterial = cmd.Material;
            currentOverrideTexture = cmdTexture;

            // --- 生成几何体数据 (CPU Transform) ---
            constexpr size_t quadVertexCount = 4;
            constexpr glm::vec4 quadVertexPos[4] = {
                { -0.5f, -0.5f, 0.0f, 1.0f },
                {  0.5f, -0.5f, 0.0f, 1.0f },
                {  0.5f,  0.5f, 0.0f, 1.0f },
                { -0.5f,  0.5f, 0.0f, 1.0f }
            };

            // 获取纹理坐标
            std::vector<glm::vec2> texCoords;
            std::vector<glm::vec2> defaultCoords = { {0,0}, {1,0}, {1,1}, {0,1} };
            
            texCoords = cmd.Texcoord;

            // 填充4个顶点
            for (size_t i = 0; i < quadVertexCount; i++) {
                s_Data.QuadVertexBufferPtr->Position = cmd.Transform * quadVertexPos[i];
                s_Data.QuadVertexBufferPtr->TexCoord = texCoords[i];
                s_Data.QuadVertexBufferPtr->Color = cmd.Color;
                s_Data.QuadVertexBufferPtr->textureIndex = 0;
                s_Data.QuadVertexBufferPtr->EntityID = (float)cmd.EntityID;
                s_Data.QuadVertexBufferPtr++;
            }

            quadCount++;
        }

        // 渲染最后一个批次
        if (quadCount > 0) {
            RenderBatch(currentMaterial, currentOverrideTexture, quadCount);
        }
    }

    void Renderer2DMaterial::RenderBatch(Ref<Material> material, Ref<Texture2D> overrideTexture, uint32_t count) {
        if (!material) return;

        // 1. 上传顶点数据到 GPU
        uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
        s_Data.QuadVBO->SetData(s_Data.QuadVertexBufferBase, dataSize);

        // 2. 绑定材质 (Shader + Uniforms + Material Textures)
        material->Bind();

        // 3. 补设全局 Uniforms (ViewProjection)
        // 注意：这需要在 material->Bind() 之后，因为 Bind() 会绑定 Shader
        // 假设 Shader 中该 Uniform 命名为 "u_ViewProjection"
        material->GetShader()->SetMatrix4("u_ProjViewMat", s_Data.ViewProjection);

        // 4. 处理纹理覆盖 (如果 DrawQuad 中传入了特定纹理)
        // 这种模式下，我们假设 Shader 只需要一个主纹理，通常在 slot 0
        // 或者我们覆盖名为 "u_Texture" 的 uniform
//        if (overrideTexture) {
//            uint32_t slot = 0; // 默认使用 slot 0 作为主纹理
//            overrideTexture->Bind(slot);
//            material->GetShader()->SetInt("u_Texture", slot);
//        }

        // 5. 发起绘制
        s_Data.QuadVAO->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVAO, count * 6);
        
        // 统计
        s_Data.Stats.DrawCalls++;
        s_Data.Stats.QuadCount += count;
    }

    // --- 提交命令接口 ---

//    void Renderer2DMaterial::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID, Ref<Material> material) {
//        s_Data.QuadCommands.push_back({
//            transform,
//            color,
//            nullptr, // No override texture
//            nullptr,
//            entityID,
//            material
//        });
//    }

    void Renderer2DMaterial::DrawQuad(const glm::mat4& transform,const std::vector<glm::vec2>& texcoord, int entityID, Ref<Material> material) {
        glm::vec4 color = material? material->GetPureColor() : glm::vec4(1.f);
        s_Data.QuadCommands.push_back({
            transform,
            color,
            texcoord,
            entityID,
            material
        });
    }

//    void Renderer2DMaterial::DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, int entityID, Ref<Material> material) {
//        s_Data.QuadCommands.push_back({
//            transform,
//            glm::vec4(1.0f),
//            texture,
//            nullptr,
//            entityID,
//            material
//        });
//    }
//
//    void Renderer2DMaterial::DrawQuad(const glm::mat4& transform, Ref<SubTexture2D> subTexture, int entityID, Ref<Material> material) {
//        s_Data.QuadCommands.push_back({
//            transform,
//            glm::vec4(1.0f),
//            nullptr,
//            subTexture,
//            entityID,
//            material
//        });
//    }

    // 获取统计信息
    Renderer2DMaterial::Statistics Renderer2DMaterial::GetStats() {
        return s_Data.Stats;
    }

    void Renderer2DMaterial::ResetStats() {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }
}
