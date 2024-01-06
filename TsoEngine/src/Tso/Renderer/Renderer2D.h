//
//  Renderer2D.hpp
//  TsoEngine
//
//  Created by SuchanTso on 2023/9/14.
//

#ifndef Renderer2D_hpp
#define Renderer2D_hpp

#include <stdio.h>
#include "OrthographicCamera.h"
#include "glm/glm.hpp"
#include "Shader.h"
#include "Texture.h"
#include "SubTexture2D.h"

namespace Tso {

class Renderer2D{
public:
    static void Init(Ref<Shader> quadShader);
    
    static void BeginScene(const OrthographicCamera& camera);
    
    static void EndScene();

    static void Flush();
    
    static void DrawQuad(const glm::vec2& position , const glm::vec2& scale , const glm::vec4& color);
    static void DrawQuad(const glm::vec3& position , const glm::vec2& scale , const glm::vec4& color);
    static void DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , const glm::vec4& color);
    
    static void DrawQuad(const glm::mat4& transform , const glm::vec4& color);
    
    static void DrawQuad(const glm::vec2& position , const glm::vec2& scale , Ref<Texture2D> texture);
    static void DrawQuad(const glm::vec3& position , const glm::vec2& scale , Ref<Texture2D> texture);
    static void DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , Ref<Texture2D> texture);
    static void DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , Ref<SubTexture2D> subTexture);
    static void DrawQuad(const glm::mat4& transform , Ref<Texture2D> texture);
    static void DrawQuad(const glm::mat4& transform , Ref<SubTexture2D> subTexture);


    struct Statistics {
        uint32_t DrawCalls = 0;
        uint32_t QuadCount = 0;

        uint32_t GetTotalVertexCount() { return QuadCount * 4; }
        uint32_t GetTotalIndexCount() { return QuadCount * 6; }

    };
    static void ResetStat();
    static Statistics GetStat();

    
private:
    static void FlushAndRest();
};

}

#endif /* Renderer2D_hpp */
