//
//  Renderer2D.cpp
//  TsoEngine
//
//  Created by SuchanTso on 2023/9/14.
//

#include "TPch.h"
#include "Renderer2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Font.h"
#include "Tso/Scene/Component.h"

namespace Tso{

struct QuadVertex {
    glm::vec3 postion;
    glm::vec2 texCoord;
    glm::vec4 color;
    float textureIndex;
    int entityID;
};

struct TextVertex{
    glm::vec3 postion;
    glm::vec2 texCoord;
    glm::vec4 color;
    int entityID;
};

    

struct Renderer2DData{

    static const uint32_t maxQuads = 2000;
    static const uint32_t maxVertices = maxQuads * 4;
    static const uint32_t maxIndices = maxQuads * 6;
    static const uint32_t maxTextureSlot = 16;
    

    Ref<Texture2D> DefaultTex;

    glm::vec4 quadVertices[4] = {
        {   -0.5f , -0.5f , 0.0f , 1.0f },
        {    0.5f , -0.5f , 0.0f , 1.0f },
        {    0.5f ,  0.5f , 0.0f , 1.0f },
        {   -0.5f ,  0.5f , 0.0f , 1.0f }
    };

    //Quad 
    Ref<VertexArray> QuadVertextArray;
    Ref<VertexBuffer> QuadVertexBuffer;
    QuadVertex* QuadVertexBufferBase = nullptr;
    QuadVertex* QuadVertexBufferPtr = nullptr;
    Ref<Shader> QuadShader;
    uint32_t QuadIndexCount = 0;
    std::array<Ref<Texture2D>, maxTextureSlot> QuadTextureSlots;
    uint32_t QuadTextureIndex = 1;//0 for white texture

    //Text
    Ref<VertexArray> TextVertextArray;
    Ref<VertexBuffer> TextVertexBuffer;
    TextVertex* TextVertexBufferBase = nullptr;
    TextVertex* TextVertexBufferPtr = nullptr;
    Ref<Shader> TextShader;
    Ref<Texture2D> FontAtlasTexture;
    uint32_t TextIndexCount = 0;

    
    //Info
    Renderer2D::Statistics Stat;
};



static Renderer2DData s_Data ;


void Renderer2D::Init(){
    Ref<IndexBuffer> indexBuffer = GenIndexBuffer(s_Data.maxIndices);


    RendererSpec quadSpec;
    quadSpec.shaderPath = std::filesystem::path("asset/shader/Shader2D.glsl");
    quadSpec.layout = {
        {ShaderDataType::Float3 , "a_Position"},
        {ShaderDataType::Float2 , "a_TexCoord"},
        {ShaderDataType::Float4 , "a_Color"},
        {ShaderDataType::Float  , "a_TexIndex"},
        {ShaderDataType::Int    , "a_EntityID"}
    };
    InitRenderer<QuadVertex>(s_Data.QuadShader, s_Data.QuadVertextArray, s_Data.QuadVertexBuffer, &s_Data.QuadVertexBufferBase , indexBuffer, quadSpec);

    RendererSpec textSpec;
    textSpec.shaderPath = std::filesystem::path("asset/shader/Text.glsl");
    textSpec.layout = {
        {ShaderDataType::Float3 , "a_Position"},
        {ShaderDataType::Float2 , "a_TexCoord"},
        {ShaderDataType::Float4 , "a_Color"},
        {ShaderDataType::Int    , "a_EntityID"}
    };
    InitRenderer<TextVertex>(s_Data.TextShader, s_Data.TextVertextArray, s_Data.TextVertexBuffer, &s_Data.TextVertexBufferBase , indexBuffer , textSpec);



    GenDefaultTexture();
    s_Data.QuadTextureSlots[0] = s_Data.DefaultTex;

    int32_t samplers[s_Data.maxTextureSlot];
    for (uint32_t i = 0; i < s_Data.maxTextureSlot; i++){
        samplers[i] = i;
    }

    s_Data.TextShader->Bind();
    s_Data.TextShader->SetInt("u_Textures", 0);

    s_Data.QuadShader->Bind();
    s_Data.QuadShader->SetIntArray("u_Textures", samplers , s_Data.maxTextureSlot);

    
}

void Renderer2D::BeginScene(const OrthographicCamera& camera){
    
    s_Data.TextShader->Bind();
    s_Data.TextShader->SetMatrix4("u_ProjViewMat", camera.GetProjViewMatrix());
    s_Data.TextVertexBufferPtr = s_Data.TextVertexBufferBase;
    s_Data.TextIndexCount = 0;
    s_Data.TextShader->UnBind();
    
    s_Data.QuadShader->Bind();
    s_Data.QuadShader->SetMatrix4("u_ProjViewMat", camera.GetProjViewMatrix());
    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
    s_Data.QuadIndexCount = 0;

    
}

void Renderer2D::BeginScene(const SceneCamera& camera, const glm::mat4& cameraTransform)
{
    s_Data.TextShader->Bind();
    s_Data.TextShader->SetMatrix4("u_ProjViewMat", camera.GetProjection() * glm::inverse(cameraTransform));
    s_Data.TextVertexBufferPtr = s_Data.TextVertexBufferBase;
    s_Data.TextIndexCount = 0;
    s_Data.TextShader->UnBind();
    
    
    s_Data.QuadShader->Bind();
    s_Data.QuadShader->SetMatrix4("u_ProjViewMat", camera.GetProjection() * glm::inverse(cameraTransform));
    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
    s_Data.QuadIndexCount = 0;

}

void Renderer2D::EndScene() {


    uint32_t vertexCount = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
    s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, vertexCount);

    uint32_t textVertexCount = (uint8_t*)s_Data.TextVertexBufferPtr - (uint8_t*)s_Data.TextVertexBufferBase;
    s_Data.TextVertexBuffer->SetData(s_Data.TextVertexBufferBase, textVertexCount);

    Flush();
}

void Renderer2D::Flush() {
    
    if(s_Data.QuadIndexCount){
        for (uint32_t i = 0; i < s_Data.QuadTextureIndex; i++) {
            s_Data.QuadTextureSlots[i]->Bind(i);
        }
        RenderCommand::DrawIndexed(s_Data.QuadVertextArray, s_Data.QuadIndexCount);
        s_Data.Stat.DrawCalls++;
    }
    
    if(s_Data.TextIndexCount){
        s_Data.FontAtlasTexture->Bind(0);
        s_Data.TextShader->Bind();
        RenderCommand::DrawIndexed(s_Data.TextVertextArray, s_Data.TextIndexCount);
        s_Data.Stat.DrawCalls++;
    }
}

Ref<IndexBuffer> Renderer2D::GenIndexBuffer(const uint32_t& maxIndexCounts)
{
    uint32_t* quadIndices = new uint32_t[maxIndexCounts];

    uint32_t offset = 0;
    for (uint32_t i = 0; i < maxIndexCounts; i += 6) {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;

        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;

        offset += 4;
    }

    Ref<IndexBuffer> indexBuffer;
    indexBuffer.reset(IndexBuffer::Create(quadIndices, s_Data.maxIndices));
    delete[]quadIndices;
    return indexBuffer;
}

template<typename T>
void Renderer2D::InitRenderer(Ref<Shader>& shader, Ref<VertexArray>& vertexArray, Ref<VertexBuffer>& vertexBuffer, T** vertexBase , Ref<IndexBuffer>& indexBuffer,const RendererSpec& rendererSpec)
{
    shader = Shader::Create(rendererSpec.shaderPath.string());
    vertexArray.reset(Tso::VertexArray::Create());
    vertexBuffer.reset(Tso::VertexBuffer::Create(s_Data.maxVertices * sizeof(T)));
    vertexBuffer->SetLayout(rendererSpec.layout);
    vertexArray->AddVertexBuffer(vertexBuffer);
    vertexArray->SetIndexBuffer(indexBuffer);
    *vertexBase = new T[s_Data.maxVertices];
}

void Renderer2D::GenDefaultTexture()
{
    s_Data.DefaultTex = Texture2D::Create(1, 1);
    uint32_t pureColor = 0xffffffff;
    s_Data.DefaultTex->SetData(&pureColor, 4);
}

void Renderer2D::FlushAndRest()
{
    EndScene();

    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
    s_Data.QuadIndexCount = 0;
    s_Data.QuadTextureIndex = 1;


    s_Data.TextVertexBufferPtr = s_Data.TextVertexBufferBase;
    s_Data.TextIndexCount = 0;

}



void Renderer2D::DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , const glm::vec4& color , const int& entityID){
    s_Data.QuadShader->Bind();

    if (s_Data.QuadIndexCount >= Renderer2DData::maxIndices) {
        FlushAndRest();
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0))
        * glm::scale(glm::mat4(1.0), glm::vec3(scale, 1.0f));

    DrawQuad(transform , color , entityID);
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color , const int& entityID){
    
    const float textureIndex = 0.0f;//white texture
    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[0];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 0.0f , 0.0f };
    s_Data.QuadVertexBufferPtr->entityID = entityID;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[1];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 1.0f , 0.0f };
    s_Data.QuadVertexBufferPtr->entityID = entityID;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[2];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 1.0f , 1.0f };
    s_Data.QuadVertexBufferPtr->entityID = entityID;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[3];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 0.0f , 1.0f };
    s_Data.QuadVertexBufferPtr->entityID = entityID;
    s_Data.QuadVertexBufferPtr++;
    
    s_Data.QuadIndexCount += 6;

    s_Data.Stat.QuadCount++;
    
}

void Renderer2D::DrawQuad(const glm::vec2& position , const glm::vec2& scale , const glm::vec4& color , const int& entityID){
    
    DrawQuad(glm::vec3(position , 0.f), 0.f, scale, color , entityID);
    
}
void Renderer2D::DrawQuad(const glm::vec3& position , const glm::vec2& scale , const glm::vec4& color , const int& entityID){
    DrawQuad(position, 0.f, scale, color , entityID);

}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, Ref<Texture2D> texture , const int& entityID)
{
    DrawQuad({position , 0.0} , 0.0 , scale ,texture , entityID);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, Ref<Texture2D> texture , const int& entityID)
{
    DrawQuad( position , 0.0, scale, texture , entityID);

}

void Renderer2D::DrawQuad(const glm::mat4& transform , Ref<Texture2D> texture , const int& entityID){
    if (s_Data.QuadIndexCount >= Renderer2DData::maxIndices) {
        FlushAndRest();
    }

    constexpr glm::vec4 color = { 1.0 , 1.0 , 1.0 , 1.0 };

    float textureIndex = 0.0f;

    for (uint32_t i = 1; i <= s_Data.QuadTextureIndex; i++) {

        if (s_Data.QuadTextureSlots[i] != nullptr && *s_Data.QuadTextureSlots[i].get() == *texture.get()) {
            textureIndex = (float)i;
            break;
        }

    }

    if (textureIndex == 0.0f) {
        textureIndex = (float)s_Data.QuadTextureIndex;
        s_Data.QuadTextureSlots[s_Data.QuadTextureIndex] = texture;
        s_Data.QuadTextureIndex++;
    }

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[0];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 0.0f , 0.0f };
    s_Data.QuadVertexBufferPtr->entityID = entityID;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[1];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 1.0f , 0.0f };
    s_Data.QuadVertexBufferPtr->entityID = entityID;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[2];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 1.0f , 1.0f };
    s_Data.QuadVertexBufferPtr->entityID = entityID;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[3];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 0.0f , 1.0f };
    s_Data.QuadVertexBufferPtr->entityID = entityID;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadIndexCount += 6;
    
    s_Data.Stat.QuadCount++;
}


void Renderer2D::DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , Ref<Texture2D> texture , const int& entityID){

    

    glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0))
        * glm::scale(glm::mat4(1.0), glm::vec3(scale, 1.0f));
    
    DrawQuad(transform , texture , entityID);
}

void Renderer2D::DrawQuad(const glm::mat4& transform , Ref<SubTexture2D> subTexture , const int& entityID){
    if (s_Data.QuadIndexCount >= Renderer2DData::maxIndices) {
        FlushAndRest();
    }
    
    Ref<Texture2D> texture = subTexture->GetTexture();

    constexpr glm::vec4 color = { 1.0 , 1.0 , 1.0 , 1.0 };

    float textureIndex = 0.0f;

    for (uint32_t i = 1; i <= s_Data.QuadTextureIndex; i++) {

        if (s_Data.QuadTextureSlots[i] != nullptr && *s_Data.QuadTextureSlots[i].get() == *texture.get()) {
            textureIndex = (float)i;
            break;
        }

    }

    if (textureIndex == 0.0f) {
        textureIndex = (float)s_Data.QuadTextureIndex;
        s_Data.QuadTextureSlots[s_Data.QuadTextureIndex] = texture;
        s_Data.QuadTextureIndex++;
    }
    
    std::vector<glm::vec2>texCoord = subTexture->GetTexCoords();

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[0];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = texCoord[0];
    s_Data.QuadVertexBufferPtr->entityID = entityID;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[1];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = texCoord[1];
    s_Data.QuadVertexBufferPtr->entityID = entityID;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[2];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = texCoord[2];
    s_Data.QuadVertexBufferPtr->entityID = entityID;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[3];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = texCoord[3];
    s_Data.QuadVertexBufferPtr->entityID = entityID;
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadIndexCount += 6;
    
    s_Data.Stat.QuadCount++;
}

void Renderer2D::DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , Ref<SubTexture2D> subTexture , const int& entityID){

    glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0))
        * glm::scale(glm::mat4(1.0), glm::vec3(scale, 1.0f));

    DrawQuad(transform , subTexture , entityID);
}

//text

void Renderer2D::DrawString(const Ref<Font> font , const glm::mat4& transform , const std::string& text ,const TextParam& textParam , const int& entityID){
    const auto& fontGeometry = font->GetMSDFData()->FontGeometry;
    const auto& metrics = fontGeometry.getMetrics();
    Ref<Texture2D> fontAtlas = font->GetAtlasTexture();
    s_Data.FontAtlasTexture = fontAtlas;
    
    double x = 0.0;//cusor
    double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
    double y = 0.0;//cusor
    
    const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();
    
    float lineSpacing = textParam.LineSpacing;
    float characterSpacing = textParam.CharacterSpacing;
    
    for(size_t i = 0 ; i < text.length() ; i++){
        
        char character = text[i];
        if(character == '\r'){
            continue;
        }
        
        if(character == '\n'){
            x = 0;
            y -= fsScale * metrics.lineHeight + lineSpacing;
            continue;
        }
        if(character == ' '){
            float advance = spaceGlyphAdvance;
            if(i < text.length() - 1){
                char nextCharacter = text[i + 1];
                double dAdvance;
                fontGeometry.getAdvance(dAdvance, character, nextCharacter);
                advance = (float)dAdvance;
            }
            x += fsScale * advance + characterSpacing;
        }
        if(character == '\t'){
            x += 4.0f * (fsScale * spaceGlyphAdvance + characterSpacing);
            continue;
        }
        
        auto glyph = fontGeometry.getGlyph(character);
        if(!glyph){
            glyph = fontGeometry.getGlyph('?');
        }
        if(!glyph){
            return;
        }
        
        double al , ab , ar , at;
        glyph->getQuadAtlasBounds(al, ab, ar, at);
        glm::vec2 texCoordMin((float)al , (float)ab);
        glm::vec2 texCoordMax((float)ar , (float)at);
        
        double pl , pb , pr , pt;
        glyph->getQuadPlaneBounds(pl, pb , pr, pt);
        glm::vec2 quadMin((float)pl , (float)pb);
        glm::vec2 quadMax((float)pr , (float)pt);
        
        quadMin *= fsScale , quadMax *= fsScale;
        quadMin += glm::vec2(x , y);
        quadMax += glm::vec2(x , y);
        
        float texelWidth = 1.0f / fontAtlas->GetWidth();
        float texelHeight = 1.0f / fontAtlas->GetHeight();
        
        texCoordMin *= glm::vec2(texelWidth , texelHeight);
        texCoordMax *= glm::vec2(texelWidth , texelHeight);
        
        
        s_Data.TextVertexBufferPtr->postion = transform * glm::vec4(quadMin , 0.0f , 1.0f);
        s_Data.TextVertexBufferPtr->color = glm::vec4(1.0);
        s_Data.TextVertexBufferPtr->texCoord = texCoordMin;
        s_Data.TextVertexBufferPtr->entityID = entityID;
        s_Data.TextVertexBufferPtr++;
        
        s_Data.TextVertexBufferPtr->postion = transform * glm::vec4(quadMin.x , quadMax.y , 0.0f , 1.0f);
        s_Data.TextVertexBufferPtr->color = glm::vec4(1.0);
        s_Data.TextVertexBufferPtr->texCoord = {texCoordMin.x , texCoordMax.y};
        s_Data.TextVertexBufferPtr->entityID = entityID;
        s_Data.TextVertexBufferPtr++;
        
        s_Data.TextVertexBufferPtr->postion = transform * glm::vec4(quadMax , 0.0f , 1.0f);
        s_Data.TextVertexBufferPtr->color = glm::vec4(1.0);
        s_Data.TextVertexBufferPtr->texCoord = texCoordMax;
        s_Data.TextVertexBufferPtr->entityID = entityID;
        s_Data.TextVertexBufferPtr++;
        
        s_Data.TextVertexBufferPtr->postion = transform * glm::vec4(quadMax.x , quadMin.y , 0.0f , 1.0f);
        s_Data.TextVertexBufferPtr->color = glm::vec4(1.0);
        s_Data.TextVertexBufferPtr->texCoord = {texCoordMax.x , texCoordMin.y};
        s_Data.TextVertexBufferPtr->entityID = entityID;
        s_Data.TextVertexBufferPtr++;

        s_Data.TextIndexCount += 6;
        s_Data.Stat.QuadCount++;
        
        if(i < text.length() - 1){
            double advance = glyph->getAdvance();
            char nextCharacter = text[i + 1];
            fontGeometry.getAdvance(advance, character, nextCharacter);
            
            x += fsScale * advance + characterSpacing;
        }
    }
    

}


void Renderer2D::ResetStat()
{
    memset(&s_Data.Stat, 0, sizeof(Renderer2D::Statistics));
}

Renderer2D::Statistics Renderer2D::GetStat()
{
    return s_Data.Stat;
}










}
