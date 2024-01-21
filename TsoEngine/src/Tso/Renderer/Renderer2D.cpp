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

namespace Tso{

struct QuadVertex {
    glm::vec3 postion;
    glm::vec2 texCoord;
    glm::vec4 color;
    float textureIndex;
};

struct TextVertex{
    glm::vec3 postion;
    glm::vec2 texCoord;
    glm::vec4 color;
};

    

struct Renderer2DData{

    static const uint32_t maxQuads = 2000;
    static const uint32_t maxVertices = maxQuads * 4;
    static const uint32_t maxIndices = maxQuads * 6;
    static const uint32_t maxTextureSlot = 16;
    
    Ref<Texture2D> DefaultTex;


    Ref<VertexArray> QuadVertextArray;
    Ref<VertexBuffer> QuadVertexBuffer;
    QuadVertex* QuadVertexBufferBase = nullptr;
    QuadVertex* QuadVertexBufferPtr = nullptr;
    Ref<Shader> QuadShader;

    uint32_t QuadIndexCount = 0;
    uint32_t TextIndexCount = 0;


    
    Ref<VertexArray> TextVertextArray;
    Ref<VertexBuffer> TextVertexBuffer;
    TextVertex* TextVertexBufferBase = nullptr;
    TextVertex* TextVertexBufferPtr = nullptr;
    Ref<Shader> TextShader;
    Ref<Texture2D> FontAtlasTexture;

    
    std::array<Ref<Texture2D>, maxTextureSlot> QuadTextureSlots;
    uint32_t QuadTextureIndex = 1;//0 for white texture

    glm::vec4 quadVertices[4] = {
        {   -0.5f , -0.5f , 0.0f , 1.0f },
        {    0.5f , -0.5f , 0.0f , 1.0f },
        {    0.5f ,  0.5f , 0.0f , 1.0f },
        {   -0.5f ,  0.5f , 0.0f , 1.0f }
    };
    

    Renderer2D::Statistics Stat;
};



static Renderer2DData s_Data ;


void Renderer2D::Init(Ref<Shader> quadShader){


    
    
    s_Data.QuadShader = Shader::Create("asset/shader/Shader2D.glsl");
    
    s_Data.TextShader = Shader::Create("asset/shader/Text.glsl");
    
    
    s_Data.QuadVertextArray.reset(Tso::VertexArray::Create());
    s_Data.TextVertextArray.reset(Tso::VertexArray::Create());



    s_Data.QuadVertexBuffer.reset(Tso::VertexBuffer::Create(s_Data.maxVertices * sizeof(QuadVertex)));

    {
        Tso::BufferLayout layout = {
            {Tso::ShaderDataType::Float3 , "a_Position"},
            {Tso::ShaderDataType::Float2 , "a_TexCoord"},
            {Tso::ShaderDataType::Float4 , "a_Color"},
            {Tso::ShaderDataType::Float  , "a_TexIndex"}


        };
        s_Data.QuadVertexBuffer->SetLayout(layout);
    }

    s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.maxVertices];

    
    s_Data.TextVertexBuffer.reset(Tso::VertexBuffer::Create(s_Data.maxVertices * sizeof(TextVertex)));

    {
        Tso::BufferLayout layout = {
            {Tso::ShaderDataType::Float3 , "a_Position"},
            {Tso::ShaderDataType::Float2 , "a_TexCoord"},
            {Tso::ShaderDataType::Float4 , "a_Color"}
        };
        s_Data.TextVertexBuffer->SetLayout(layout);
    }

    s_Data.TextVertexBufferBase = new TextVertex[s_Data.maxVertices];

    uint32_t* quadIndices = new uint32_t[s_Data.maxIndices];

    uint32_t offset = 0;
    for (uint32_t i = 0; i < s_Data.maxIndices; i += 6) {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;

        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;

        offset += 4;
    }

    Tso::Ref<Tso::IndexBuffer> indexBuffer;
    indexBuffer.reset(Tso::IndexBuffer::Create(quadIndices, s_Data.maxIndices));

    s_Data.QuadVertextArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
    s_Data.QuadVertextArray->SetIndexBuffer(indexBuffer);
    
    s_Data.TextVertextArray->AddVertexBuffer(s_Data.TextVertexBuffer);
    s_Data.TextVertextArray->SetIndexBuffer(indexBuffer);

    delete []quadIndices;
    
    s_Data.DefaultTex = Texture2D::Create(1,1);
    uint32_t pureColor = 0xffffffff;
    s_Data.DefaultTex->SetData(&pureColor , 4);
    

      s_Data.QuadTextureSlots[0] = s_Data.DefaultTex;

      int32_t samplers[s_Data.maxTextureSlot];
      for (uint32_t i = 0; i < s_Data.maxTextureSlot; i++) {
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

    
    s_Data.QuadTextureIndex = 1;
    while(s_Data.QuadTextureIndex < s_Data.QuadTextureSlots.size()){
        if(s_Data.QuadTextureSlots[s_Data.QuadTextureIndex] == nullptr){
            break;
        }
        s_Data.QuadTextureIndex++;
    }
    
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


    s_Data.QuadTextureIndex = 1;
    while (s_Data.QuadTextureIndex < s_Data.QuadTextureSlots.size()) {
        if (s_Data.QuadTextureSlots[s_Data.QuadTextureIndex] == nullptr) {
            break;
        }
        s_Data.QuadTextureIndex++;
    }

}

void Renderer2D::EndScene() {


    uint32_t vertexCount = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
    s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, vertexCount);

    uint32_t textVertexCount = (uint8_t*)s_Data.TextVertexBufferPtr - (uint8_t*)s_Data.TextVertexBufferBase;
    
    s_Data.TextVertexBuffer->SetData(s_Data.TextVertexBufferBase, textVertexCount);

    Flush();
}

void Renderer2D::Flush() {
    for (uint32_t i = 0; i < s_Data.QuadTextureIndex; i++) {
        s_Data.QuadTextureSlots[i]->Bind(i);
    }
    if(s_Data.QuadIndexCount){
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

void Renderer2D::FlushAndRest()
{
    EndScene();

    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

    s_Data.QuadIndexCount = 0;

    while(s_Data.QuadTextureIndex < s_Data.QuadTextureSlots.size()){
        if(s_Data.QuadTextureSlots[s_Data.QuadTextureIndex] == nullptr){
            break;
        }
        s_Data.QuadTextureIndex++;
    }
}



void Renderer2D::DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , const glm::vec4& color){
    s_Data.QuadShader->Bind();

    if (s_Data.QuadIndexCount >= Renderer2DData::maxIndices) {
        FlushAndRest();
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0))
        * glm::scale(glm::mat4(1.0), glm::vec3(scale, 1.0f));

    DrawQuad(transform , color);
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color){
    
    const float textureIndex = 0.0f;//white texture
    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[0];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 0.0f , 0.0f };
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[1];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 1.0f , 0.0f };
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[2];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 1.0f , 1.0f };
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[3];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 0.0f , 1.0f };
    s_Data.QuadVertexBufferPtr++;
    
    s_Data.QuadIndexCount += 6;

    s_Data.Stat.QuadCount++;
    
}

void Renderer2D::DrawQuad(const glm::vec2& position , const glm::vec2& scale , const glm::vec4& color){
    
    DrawQuad(glm::vec3(position , 0.f), 0.f, scale, color);
    
}
void Renderer2D::DrawQuad(const glm::vec3& position , const glm::vec2& scale , const glm::vec4& color){
    DrawQuad(position, 0.f, scale, color);

}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, Ref<Texture2D> texture)
{
    DrawQuad({position , 0.0} , 0.0 , scale ,texture);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, Ref<Texture2D> texture)
{
    DrawQuad( position , 0.0, scale, texture);

}

void Renderer2D::DrawQuad(const glm::mat4& transform , Ref<Texture2D> texture){
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
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[1];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 1.0f , 0.0f };
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[2];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 1.0f , 1.0f };
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[3];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = { 0.0f , 1.0f };
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadIndexCount += 6;
    
    s_Data.Stat.QuadCount++;
}


void Renderer2D::DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , Ref<Texture2D> texture){

    

    glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0))
        * glm::scale(glm::mat4(1.0), glm::vec3(scale, 1.0f));
    
    DrawQuad(transform , texture);
}

void Renderer2D::DrawQuad(const glm::mat4& transform , Ref<SubTexture2D> subTexture){
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
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[1];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = texCoord[1];
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[2];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = texCoord[2];
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadVertexBufferPtr->postion = transform * s_Data.quadVertices[3];
    s_Data.QuadVertexBufferPtr->color = color;
    s_Data.QuadVertexBufferPtr->textureIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->texCoord = texCoord[3];
    s_Data.QuadVertexBufferPtr++;

    s_Data.QuadIndexCount += 6;
    
    s_Data.Stat.QuadCount++;
}

void Renderer2D::DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , Ref<SubTexture2D> subTexture){

    glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0))
        * glm::scale(glm::mat4(1.0), glm::vec3(scale, 1.0f));

    DrawQuad(transform , subTexture);
}

//text

void Renderer2D::DrawString(const Ref<Font> font , const glm::mat4& transform , const std::string& text){
    const auto& fontGeometry = font->GetMSDFData()->FontGeometry;
    const auto& metrics = fontGeometry.getMetrics();
    Ref<Texture2D> fontAtlas = font->GetAtlasTexture();
    s_Data.FontAtlasTexture = fontAtlas;
    
    double x = 0.0;//cusor
    double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
    double y = 0.0;//cusor
    
    const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();
    
    float lineSpacing = 0.0;//temp
    float characterSpacing = 0.0;//temp
    
    for(size_t i = 0 ; i < text.length() ; i++){
        
        char character = text[i];
        if(character == '\r'){
            continue;
        }
        
        if(character == '\n'){
            x = 0;
            y -= fsScale * metrics.lineHeight ;
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
        s_Data.TextVertexBufferPtr++;
        
        s_Data.TextVertexBufferPtr->postion = transform * glm::vec4(quadMin.x , quadMax.y , 0.0f , 1.0f);
        s_Data.TextVertexBufferPtr->color = glm::vec4(1.0);
        s_Data.TextVertexBufferPtr->texCoord = {texCoordMin.x , texCoordMax.y};
        s_Data.TextVertexBufferPtr++;
        
        s_Data.TextVertexBufferPtr->postion = transform * glm::vec4(quadMax , 0.0f , 1.0f);
        s_Data.TextVertexBufferPtr->color = glm::vec4(1.0);
        s_Data.TextVertexBufferPtr->texCoord = texCoordMax;
        s_Data.TextVertexBufferPtr++;
        
        s_Data.TextVertexBufferPtr->postion = transform * glm::vec4(quadMax.x , quadMin.y , 0.0f , 1.0f);
        s_Data.TextVertexBufferPtr->color = glm::vec4(1.0);
        s_Data.TextVertexBufferPtr->texCoord = {texCoordMax.x , texCoordMin.y};
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
