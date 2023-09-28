//
//  Renderer2D.cpp
//  TsoEngine
//
//  Created by SuchanTso on 2023/9/14.
//

#include "TPch.h"
#include "Renderer2D.h"
#include "glm/gtc/matrix_transform.hpp"


namespace Tso{

struct Renderer2DData{
    Ref<VertexArray> QuadVertextArray;
    Ref<Shader> QuadShader;
    Ref<Texture2D> DefaultTex;

};

Renderer2DData* s_Data = nullptr;


void Renderer2D::Init(Ref<Shader> quadShader){
    s_Data = new Renderer2DData();
    
    s_Data->QuadShader = quadShader;
    
    s_Data->QuadVertextArray.reset(Tso::VertexArray::Create());

    float vertices[5 * 4] = {
        -0.5f , -0.5f , 0.0f , 0.0f , 0.0f,
         0.5f , -0.5f , 0.0f , 1.0f , 0.0f,
        -0.5f ,  0.5f , 0.0f , 0.0f , 1.0f,
         0.5f ,  0.5f , 0.0f , 1.0f , 1.0f
    };

    Tso::Ref<Tso::VertexBuffer> vertexBuffer;
    vertexBuffer.reset(Tso::VertexBuffer::Create(vertices, sizeof(vertices)));

    {
        Tso::BufferLayout layout = {
            {Tso::ShaderDataType::Float3 , "a_Position"},
            {Tso::ShaderDataType::Float2 , "a_TexCoord"}
        };
        vertexBuffer->SetLayout(layout);
    }

    uint32_t indices[6] = { 0 , 1 , 2 , 1 , 3 , 2};

    Tso::Ref<Tso::IndexBuffer> indexBuffer;
    indexBuffer.reset(Tso::IndexBuffer::Create(indices, 6));

    s_Data->QuadVertextArray->AddVertexBuffer(vertexBuffer);
    s_Data->QuadVertextArray->SetIndexBuffer(indexBuffer);
    
    s_Data->DefaultTex = Texture2D::Create(1,1);
    uint32_t pureColor = 0xffffffff;
    s_Data->DefaultTex->SetData(&pureColor);
    
    
}

void Renderer2D::BeginScene(const OrthographicCamera& camera){
    
    s_Data->QuadShader->Bind();
    
    s_Data->QuadShader->SetMatrix4("u_ProjViewMat", camera.GetProjViewMatrix());
    
}

void Renderer2D::DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , const glm::vec4& color){
    s_Data->QuadShader->Bind();
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(scale , 1.0f));
    glm::mat4 rotateMat = glm::rotate(scaleMat, glm::radians(rotation), glm::vec3(0.f , 0.f , 1.f));
    glm::mat4 transform = glm::translate(rotateMat, position);
    
    s_Data->QuadShader->SetMatrix4("u_Transform", transform);
    
    s_Data->QuadShader->SetFloat4("u_Color", color);
    
    s_Data->QuadShader->SetInt("u_Texture", 0);
    
    s_Data->DefaultTex->Bind();
    
    RenderCommand::DrawIndexed(s_Data->QuadVertextArray);
    
    s_Data->QuadShader->UnBind();
    
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

void Renderer2D::DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , Ref<Texture2D> texture){
    s_Data->QuadShader->Bind();
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(scale , 1.0f));
    glm::mat4 rotateMat = glm::rotate(scaleMat, glm::radians(rotation), glm::vec3(0.f , 0.f , 1.f));
    glm::mat4 transform = glm::translate(rotateMat, position);
    
    s_Data->QuadShader->SetMatrix4("u_Transform", transform);
    
    glm::vec4 defaultColor = glm::vec4(1.0 , 1.0 , 1.0 , 1.0);
    
    s_Data->QuadShader->SetFloat4("u_Color", defaultColor);
    
    s_Data->QuadShader->SetInt("u_Texture", 0);
    
    texture->Bind();
    
    RenderCommand::DrawIndexed(s_Data->QuadVertextArray);
    
    s_Data->QuadShader->UnBind();
}


void Renderer2D::EndScene(){
    
}




}
