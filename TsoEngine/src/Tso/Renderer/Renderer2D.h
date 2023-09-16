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

namespace Tso {

class Renderer2D{
public:
    static void Init(Ref<Shader> quadShader);
    
    static void ShutDown();
    
    static void BeginScene(const OrthographicCamera& camera);
    
    static void EndScene();
    
    static void DrawQuad(const glm::vec2& position , const glm::vec2& scale , const glm::vec4& color);
    static void DrawQuad(const glm::vec3& position , const glm::vec2& scale , const glm::vec4& color);
    static void DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , const glm::vec4& color);
    
    static void DrawQuad(const glm::vec2& position , const glm::vec2& scale , Ref<Texture2D> texture);
    static void DrawQuad(const glm::vec3& position , const glm::vec2& scale , Ref<Texture2D> texture);
    static void DrawQuad(const glm::vec3& position , const float& rotation , const glm::vec2& scale , Ref<Texture2D> texture);


    
private:

};

}

#endif /* Renderer2D_hpp */
