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
#include "Tso/Scene/Component.h"

namespace Tso {

    // --- 内部数据结构定义 ---

    struct QuadVertex {
        glm::vec3 Position;
        glm::vec2 TexCoord;
        glm::vec4 Color;
        float textureIndex;
        float EntityID;
        glm::vec4 CustomData;

    };


    struct TextVertex{
        glm::vec3 Position;
        glm::vec2 TexCoord;
        glm::vec4 Color;
        int EntityID;
        glm::vec4 CustomData;
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
        
        TextVertex* TextVertexBufferBase = nullptr;
        TextVertex* TextVertexBufferPtr = nullptr;
        
        // 统计数据
        Renderer2DMaterial::Statistics Stats;

        // 场景状态
        glm::mat4 ViewProjection;
        bool SceneBegun = false;

        // 命令队列
        std::vector<DrawQuadCommand> QuadCommands;
        std::vector<DrawStringCommand> StringCommands; // 暂时保留，逻辑类似Quad

//        Renderer2DMaterialData() {
//            QuadVertexBufferBase = new QuadVertex[MaxVertices];
//            TextVertexBufferBase = new TextVertex[MaxVertices];
//        }

        ~Renderer2DMaterialData() {
            delete[] QuadVertexBufferBase;
            delete[] TextVertexBufferBase;
        }
    };

    static Renderer2DMaterialData s_Data;

    // --- 初始化与销毁 ---

    template<typename T>
    void Renderer2DMaterial::InitRenderer(Ref<VertexArray>& vertexArray, Ref<VertexBuffer>& vertexBuffer, T** vertexBase , Ref<IndexBuffer>& indexBuffer,const BufferLayout& layout){
        
        vertexArray = VertexArray::Create();
        vertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(T));
        vertexBuffer->SetLayout(layout);
        vertexArray->AddVertexBuffer(vertexBuffer);
        vertexArray->SetIndexBuffer(indexBuffer);
        *vertexBase = new T[s_Data.MaxVertices];
    }

    void Renderer2DMaterial::Init() {
        s_Data.QuadCommands.reserve(1024);
        s_Data.StringCommands.reserve(1024);

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
        Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
        
        BufferLayout quadLayout = {
                        { ShaderDataType::Float3, "a_Position" },
                        { ShaderDataType::Float2, "a_TexCoord" },
                        { ShaderDataType::Float4, "a_Color" },
                        {ShaderDataType::Float  , "a_TexIndex"},
                        { ShaderDataType::Float, "a_EntityID" },
                        { ShaderDataType::Float4, "a_CustomData"}
        };
        BufferLayout textLayout = {
                        {ShaderDataType::Float3 , "a_Position"},
                        {ShaderDataType::Float2 , "a_TexCoord"},
                        {ShaderDataType::Float4 , "a_Color"},
                        {ShaderDataType::Int    , "a_EntityID"},
                        { ShaderDataType::Float4, "a_CustomData"}
        };
        InitRenderer<QuadVertex>(s_Data.QuadVAO, s_Data.QuadVBO, &s_Data.QuadVertexBufferBase, indexBuffer, quadLayout);
        InitRenderer<TextVertex>(s_Data.TextVAO, s_Data.TextVBO, &s_Data.TextVertexBufferBase, indexBuffer, textLayout);
        delete[] quadIndices;

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
        s_Data.TextVertexBufferPtr = s_Data.TextVertexBufferBase;
        s_Data.QuadCommands.clear();
        s_Data.StringCommands.clear();
        s_Data.Stats = Statistics();
    }

    void Renderer2DMaterial::Flush() {
        FlushQuad();
        FlushString();
    }

    void Renderer2DMaterial::FlushQuad(){
        if (s_Data.QuadCommands.empty())
            return;

        std::sort(s_Data.QuadCommands.begin(), s_Data.QuadCommands.end(),
            [](const DrawQuadCommand& a, const DrawQuadCommand& b) {
                return a.Material < b.Material;
            });

        Ref<Material> currentMaterial = nullptr;
        Ref<Texture2D> currentOverrideTexture = nullptr;
        uint32_t quadCount = 0;

        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        for (const auto& cmd : s_Data.QuadCommands) {
            if (!cmd.Material) continue;

            bool materialChanged = (currentMaterial != cmd.Material);
            bool bufferFull = (quadCount >= Renderer2DMaterialData::MaxQuads);

            if (quadCount > 0 && (materialChanged || bufferFull)) {
                RenderQuadBatch(currentMaterial , quadCount);
                quadCount = 0;
                s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
            }

            currentMaterial = cmd.Material;

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
            for (size_t i = 0; i < quadVertexCount; i++) {
                s_Data.QuadVertexBufferPtr->Position = cmd.Transform * quadVertexPos[i];
                s_Data.QuadVertexBufferPtr->TexCoord = texCoords[i];
                s_Data.QuadVertexBufferPtr->Color = cmd.Color;
                s_Data.QuadVertexBufferPtr->textureIndex = 0;
                s_Data.QuadVertexBufferPtr->EntityID = (float)cmd.EntityID;
                s_Data.QuadVertexBufferPtr->CustomData = cmd.custumData;
                s_Data.QuadVertexBufferPtr++;
            }

            quadCount++;
        }

        // last batch to render
        if (quadCount > 0) {
            RenderQuadBatch(currentMaterial , quadCount);
        }
    }

void Renderer2DMaterial::FlushString(){
    std::stable_sort(s_Data.StringCommands.begin(), s_Data.StringCommands.end(), [](const auto& a, const auto& b) {
            return a.Material < b.Material; // 简单按材质指针排序
        });

    Ref<Material> currentMaterial = nullptr;
    uint32_t quadCount = 0;

    s_Data.TextVertexBufferPtr = s_Data.TextVertexBufferBase;

    for (const auto& cmd : s_Data.StringCommands) {
        if (!cmd.Material) continue;

        bool materialChanged = (currentMaterial != cmd.Material);
        bool bufferFull = (quadCount >= Renderer2DMaterialData::MaxQuads);

        if (quadCount > 0 && (materialChanged || bufferFull)) {
            RenderTextBatch(currentMaterial , quadCount);
            quadCount = 0;
            s_Data.TextVertexBufferPtr = s_Data.TextVertexBufferBase;
        }

        currentMaterial = cmd.Material;

        quadCount += GenerateTextVertices(cmd);
    }

    // render last batch render
    if (quadCount > 0) {
        RenderTextBatch(currentMaterial , quadCount);
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
        material->GetShader()->SetMatrix4("u_ProjViewMat", s_Data.ViewProjection);

        // 4. 绘制
        s_Data.QuadVAO->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVAO, count * 6);
        
        // 统计
        s_Data.Stats.DrawCalls++;
        s_Data.Stats.QuadCount += count;
    }

    void Renderer2DMaterial::RenderQuadBatch(Ref<Material> material, uint32_t count){
        if (!material) return;

        // 1. 上传顶点数据到 GPU
        uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
        s_Data.QuadVBO->SetData(s_Data.QuadVertexBufferBase, dataSize);

        // 2. 绑定材质 (Shader + Uniforms + Material Textures)
        material->Bind();

        // 3. 补设全局 Uniforms (ViewProjection)
        material->GetShader()->SetMatrix4("u_ProjViewMat", s_Data.ViewProjection);

        // 4. 绘制
        s_Data.QuadVAO->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVAO, count * 6);
        
        // 统计
        s_Data.Stats.DrawCalls++;
        s_Data.Stats.QuadCount += count;
    }

    void Renderer2DMaterial::RenderTextBatch(Ref<Material> material, uint32_t count){
        if (!material) return;

        // 1. 上传顶点数据到 GPU
        uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.TextVertexBufferPtr - (uint8_t*)s_Data.TextVertexBufferBase);
        s_Data.TextVBO->SetData(s_Data.TextVertexBufferBase, dataSize);

        // 2. 绑定材质 (Shader + Uniforms + Material Textures)
        material->Bind();

        // 3. 补设全局 Uniforms (ViewProjection)
        material->GetShader()->SetMatrix4("u_ProjViewMat", s_Data.ViewProjection);

        // 4. 绘制
        s_Data.TextVAO->Bind();
        RenderCommand::DrawIndexed(s_Data.TextVAO, count * 6);
        
        // 统计
        s_Data.Stats.DrawCalls++;
        s_Data.Stats.QuadCount += count;
    }



    void Renderer2DMaterial::DrawQuad(const glm::mat4& transform,const std::vector<glm::vec2>& texcoord, int entityID, Ref<Material> material , MaterialInstanceComponent* overrideComp) {
        glm::vec4 color = material? material->GetPureColor() : glm::vec4(1.f);
        glm::vec4 instanceData(0.0f);

        // 如果有组件，并且材质定义了映射规则
        if (overrideComp && material) {
            // 遍历材质定义的所有实例参数
            for (const auto& [name, slot] : material->GetInstanceParams()) {
                // 查找组件里是否有这个值
                if (overrideComp->FloatOverrides.find(name) != overrideComp->FloatOverrides.end()) {
                    // 填入对应的槽位
                    instanceData[slot] = overrideComp->FloatOverrides.at(name);
                }
            }
        }
        s_Data.QuadCommands.push_back({
            transform,
            color,
            texcoord,
            entityID,
            material,
            instanceData
        });
    }

void Renderer2DMaterial::DrawString(const Ref<Font> font , const glm::mat4& transform , const std::string& text , const TextParam& textParam , const int& entityID, Ref<Material> material,MaterialInstanceComponent* overrideComp){
    if (text.empty() || !font || !material) return;

    // 解析 Instance 参数 (计算出每个顶点共用的 CustomData)
    glm::vec4 instanceData(0.0f);
    if (overrideComp) {
        for (const auto& [name, slot] : material->GetInstanceParams()) {
            if (overrideComp->FloatOverrides.find(name) != overrideComp->FloatOverrides.end()) {
                instanceData[slot] = overrideComp->FloatOverrides.at(name);
            }
        }
    }

    // 将请求存入命令队列
    s_Data.StringCommands.push_back({
        font,
        transform,
        text,
        glm::vec4(1.f),
        textParam,
        entityID,
        material,
        instanceData
    });
}

// 辅助函数：直接向 s_QuadVertexBufferPtr 写入数据
uint32_t Renderer2DMaterial::GenerateTextVertices(const DrawStringCommand& cmd) {
    const auto& fontGeometry = cmd.Font->GetMSDFData()->FontGeometry;
    const auto& metrics = fontGeometry.getMetrics();
    Ref<Texture2D> fontAtlas = cmd.Font->GetAtlasTexture();
    cmd.Material->SetTexture("u_Textures", fontAtlas);
    
    // 材质覆盖：确保 Shader 使用了这个 Atlas
    // 注意：这里有个潜规则，Material 必须预先绑定好 FontAtlas 到某个 Slot
    // 或者我们在 DrawCall 前临时 SetInt("u_Texture", slot)

    double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
    const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();
    float lineSpacing = cmd.TextParam.LineSpacing;
    float characterSpacing = cmd.TextParam.CharacterSpacing;

    // --- 第一次遍历 (计算 Bounds) ---
    // (逻辑保持不变，直接复制你的代码)
    glm::vec2 minBounds(std::numeric_limits<float>::max());
    glm::vec2 maxBounds(std::numeric_limits<float>::lowest());
    double x = 0.0, y = 0.0;
    
    for (size_t i = 0; i < cmd.Text.length(); i++) {
        char character = cmd.Text[i];
        if (character == '\r') continue;

        if (character == '\n') {
            x = 0;
            y -= fsScale * metrics.lineHeight + lineSpacing;
            continue;
        }
        
        // 特殊字符处理
        if (character == ' ') {
            float advance = spaceGlyphAdvance;
            if (i < cmd.Text.length() - 1) {
                char nextCharacter = cmd.Text[i + 1];
                double dAdvance;
                fontGeometry.getAdvance(dAdvance, character, nextCharacter);
                advance = (float)dAdvance;
            }
            x += fsScale * advance + characterSpacing;
            continue; // 空格不产生几何体，所以continue
        }
        if (character == '\t') {
            x += 4.0f * (fsScale * spaceGlyphAdvance + characterSpacing);
            continue; // 制表符也不产生几何体
        }

        auto glyph = fontGeometry.getGlyph(character);
        if (!glyph) glyph = fontGeometry.getGlyph('?');
        if (!glyph) continue;

        double pl, pb, pr, pt;
        glyph->getQuadPlaneBounds(pl, pb, pr, pt);
        glm::vec2 quadMin((float)pl, (float)pb);
        glm::vec2 quadMax((float)pr, (float)pt);

        quadMin *= fsScale;
        quadMax *= fsScale;
        quadMin += glm::vec2(x, y);
        quadMax += glm::vec2(x, y);
        
        minBounds.x = std::min(minBounds.x, quadMin.x);
        minBounds.y = std::min(minBounds.y, quadMin.y);
        maxBounds.x = std::max(maxBounds.x, quadMax.x);
        maxBounds.y = std::max(maxBounds.y, quadMax.y);

        if (i < cmd.Text.length() - 1) {
            double advance = glyph->getAdvance();
            char nextCharacter = cmd.Text[i + 1];
            fontGeometry.getAdvance(advance, character, nextCharacter);
            x += fsScale * advance + characterSpacing;
        }
    }
    
    if (minBounds.x > maxBounds.x) return 0;

    glm::vec2 textBoundsSize = maxBounds - minBounds;
    float scaleX = (textBoundsSize.x > 0.0001f) ? (1.0f / textBoundsSize.x) : 1.0f;
    float scaleY = (textBoundsSize.y > 0.0001f) ? (1.0f / textBoundsSize.y) : 1.0f;
    glm::vec2 scaleVec(std::min(scaleX,1.f), std::min(scaleY,1.f));

    // --- 第二次遍历 (生成顶点) ---
    x = 0.0; y = 0.0;
    const std::string& text = cmd.Text;
    uint32_t quadCount = 0;

    for (size_t i = 0; i < text.length(); i++) {
        char character = text[i];
        if (character == '\r') continue;

        if (character == '\n') {
            x = 0;
            y -= fsScale * metrics.lineHeight + lineSpacing;
            continue;
        }
        
        // 特殊字符处理
        if (character == ' ') {
            float advance = spaceGlyphAdvance;
            if (i < cmd.Text.length() - 1) {
                char nextCharacter = cmd.Text[i + 1];
                double dAdvance;
                fontGeometry.getAdvance(dAdvance, character, nextCharacter);
                advance = (float)dAdvance;
            }
            x += fsScale * advance + characterSpacing;
            continue; // 空格不产生几何体，所以continue
        }
        if (character == '\t') {
            x += 4.0f * (fsScale * spaceGlyphAdvance + characterSpacing);
            continue; // 制表符也不产生几何体
        }

        auto glyph = fontGeometry.getGlyph(character);
        if (!glyph) glyph = fontGeometry.getGlyph('?');
        if (!glyph) continue;

        // 计算 UV 和 Position
        double al, ab, ar, at;
        glyph->getQuadAtlasBounds(al, ab, ar, at);
        glm::vec2 texCoordMin((float)al, (float)ab);
        glm::vec2 texCoordMax((float)ar, (float)at);

        double pl, pb, pr, pt;
        glyph->getQuadPlaneBounds(pl, pb, pr, pt);
        glm::vec2 quadMin((float)pl, (float)pb);
        glm::vec2 quadMax((float)pr, (float)pt);

        quadMin *= fsScale; quadMax *= fsScale;
        quadMin += glm::vec2(x, y); quadMax += glm::vec2(x, y);

        // 归一化
        glm::vec2 normQuadMin = (quadMin - minBounds) * scaleVec - glm::vec2(0.5f);
        glm::vec2 normQuadMax = (quadMax - minBounds) * scaleVec - glm::vec2(0.5f);

        float texelWidth = 1.0f / fontAtlas->GetWidth();
        float texelHeight = 1.0f / fontAtlas->GetHeight();
        texCoordMin *= glm::vec2(texelWidth, texelHeight);
        texCoordMax *= glm::vec2(texelWidth, texelHeight);
        
        // 颜色：优先使用 TextParam 中的颜色，或者 CustomData 中的颜色？
        // 假设 TextParam.Color 存在
        glm::vec4 color = cmd.Color;

        // 写入顶点 (s_QuadVertexBufferPtr 是 Renderer 里的全局/静态指针)
        // 顶点 0
        s_Data.TextVertexBufferPtr->Position = cmd.Transform * glm::vec4(normQuadMin, 0.0f, 1.0f);
        s_Data.TextVertexBufferPtr->Color = color;
        s_Data.TextVertexBufferPtr->TexCoord = texCoordMin;
        s_Data.TextVertexBufferPtr->EntityID = (float)cmd.EntityID;
        s_Data.TextVertexBufferPtr->CustomData = cmd.CustumData; // <--- 关键：写入实例数据
        s_Data.TextVertexBufferPtr++;

        // 顶点 1
        s_Data.TextVertexBufferPtr->Position = cmd.Transform * glm::vec4(normQuadMin.x, normQuadMax.y, 0.0f, 1.0f);
        s_Data.TextVertexBufferPtr->Color = color;
        s_Data.TextVertexBufferPtr->TexCoord = {texCoordMin.x, texCoordMax.y};
        s_Data.TextVertexBufferPtr->EntityID = (float)cmd.EntityID;
        s_Data.TextVertexBufferPtr->CustomData = cmd.CustumData;
        s_Data.TextVertexBufferPtr++;

        // 顶点 2
        s_Data.TextVertexBufferPtr->Position = cmd.Transform * glm::vec4(normQuadMax, 0.0f, 1.0f);
        s_Data.TextVertexBufferPtr->Color = color;
        s_Data.TextVertexBufferPtr->TexCoord = texCoordMax;
        s_Data.TextVertexBufferPtr->EntityID = (float)cmd.EntityID;
        s_Data.TextVertexBufferPtr->CustomData = cmd.CustumData;
        s_Data.TextVertexBufferPtr++;

        // 顶点 3
        s_Data.TextVertexBufferPtr->Position = cmd.Transform * glm::vec4(normQuadMax.x, normQuadMin.y, 0.0f, 1.0f);
        s_Data.TextVertexBufferPtr->Color = color;
        s_Data.TextVertexBufferPtr->TexCoord = {texCoordMax.x, texCoordMin.y};
        s_Data.TextVertexBufferPtr->EntityID = (float)cmd.EntityID;
        s_Data.TextVertexBufferPtr->CustomData = cmd.CustumData;
        s_Data.TextVertexBufferPtr++;

        // 索引计数增加 6
//        s_Data.Stats.QuadIndexCount += 6;
        quadCount += 6;
        
        // 字符步进
        if (i < text.length() - 1) {
            double advance = glyph->getAdvance();
            char nextCharacter = text[i + 1];
            fontGeometry.getAdvance(advance, character, nextCharacter);
            x += fsScale * advance + characterSpacing;
        }
    }
    return quadCount;
}



    // 获取统计信息
    Renderer2DMaterial::Statistics Renderer2DMaterial::GetStats() {
        return s_Data.Stats;
    }

    void Renderer2DMaterial::ResetStats() {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }
}
