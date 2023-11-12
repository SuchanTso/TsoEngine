#include "SandBox2D.h"
#include "imgui.h"




SandBox2D::SandBox2D()
	:Layer("testLayer"),
    m_CameraController(1280.0 / 720, true),
    m_TrianglePos(glm::vec3(0.f))
{
    m_ShaderLibrary = std::make_shared<Tso::ShaderLibrary>();

    m_Shader = m_ShaderLibrary->Load("asset/shader/Shader2D.glsl");
    Tso::Renderer2D::Init(m_Shader);
    
    std::string lp = "asset/lp2.png";
    
    std::string tileMap = "asset/tilemap_packed.png";


    m_Texture = Tso::Texture2D::Create(lp);
    m_TileTexture = Tso::Texture2D::Create(tileMap);
    m_subTexture = Tso::SubTexture2D::CreateByCoord(m_TileTexture , {16.0 , 16.0} , {2.0 , 3.0} , {1.0 , 1.0});
    m_sub1 = Tso::SubTexture2D::CreateByCoord(m_TileTexture , {16.0 , 16.0} , {0.0 , 5.0} , {1.0 , 1.0});

    Tso::FrameBufferInfo info = {720 , 1280 , false};
    m_FrameBuffer = Tso::FrameBuffer::Create(info);

    m_TrianglePos = glm::vec3(0.0 , 0.0 , 0.1);
    
    m_MoveData.startTime = m_Time;
    m_MoveData.targetPos = glm::vec2(0.0 , 0.0);
    m_MoveData.originPos = glm::vec2(0.0 , 0.0);
}


void SandBox2D::OnImGuiRender()
{
    ImGui::Begin("SandBox2D");

    ImGui::Text("Render2DInfo");

    auto stat = Tso::Renderer2D::GetStat();

    ImGui::Text("DrawCalls : %d " , stat.DrawCalls);
    ImGui::Text("QuadVertices : %d", stat.GetTotalVertexCount());
    ImGui::Text("QuadIndices : %d" , stat.GetTotalIndexCount());
    uint32_t fbId = m_FrameBuffer->GetColorAttachment();
    ImGui::Image((void*)fbId, ImVec2{ 360.f , 360.f });

    ImGui::End();
}


template<typename T>
T SandBox2D::LinearInterpretMove(const float & start , const float& duration , const float& timestamp ,const T& pos , const T& target , bool& movable){
    if(timestamp > start + duration){
        movable = false;
        return target;
    }
    if(!movable){
        return pos;
    }
    float ratio = (timestamp - start) / duration;
    T res = (target - pos) * ratio + pos;
//    TSO_INFO("moving , start from {0} , duration is {1} , time = {2}" , start , duration , timestamp);

    return res;
}


void SandBox2D::OnUpdate(Tso::TimeStep ts)
{

    Tso::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
    Tso::RenderCommand::Clear();

    if (Tso::Input::IsKeyPressed(TSO_KEY_I)) {
        m_TrianglePos.y += m_MoveSpeed * ts;
        TSO_INFO("moved Triangle at {0}" , m_TrianglePos.y);

    }
    if (Tso::Input::IsKeyPressed(TSO_KEY_K)) {
        m_TrianglePos.y -= m_MoveSpeed * ts;
        TSO_INFO("moved Triangle at {0}" , m_TrianglePos.y);

    }
    if (Tso::Input::IsKeyPressed(TSO_KEY_J)) {
        m_TrianglePos.x -= m_MoveSpeed * ts;
        TSO_INFO("moved Triangle at {0}" , m_TrianglePos.x);

    }
    if (Tso::Input::IsKeyPressed(TSO_KEY_L)) {
        m_TrianglePos.x += m_MoveSpeed * ts;
        TSO_INFO("moved Triangle at {0}" , m_TrianglePos.x);
    }
    if (Tso::Input::IsKeyPressed(TSO_KEY_SPACE)) {
        m_MoveData.startTime = m_Time;
        m_MoveData.targetPos = glm::vec2(0.5 , 0.5);
        m_MoveData.originPos = m_TrianglePos;
         m_LpMovable = true;
    }

    Tso::Renderer2D::ResetStat();

    m_CameraController.OnUpdate(ts);
    Tso::Renderer2D::BeginScene(m_CameraController.GetCamera());
   /* Tso::Renderer2D::DrawQuad({-0.5 , 0.0 , 0.0}, 45.f, {0.5 , 0.5}, {0.8 , 0.3 , 0.2 , 1.0});
    Tso::Renderer2D::DrawQuad({0.0 , 0.0 , 0.0}, 0.f, {1.0 , 1.0}, {0.2 , 0.8 , 0.3 , 1.0});*/
    Tso::Renderer2D::DrawQuad({0.5 , 0.5 , 0.0} , 0.f , {0.5 , 0.5} , m_TileTexture);
    Tso::Renderer2D::DrawQuad({1.5 , 0.5 , 0.0} , 0.f , {0.5 , 0.5} , m_subTexture);
    Tso::Renderer2D::DrawQuad({1.5 , -0.5 , 0.0} , 0.f , {0.2 , 0.2} , m_sub1);



     
//    if(m_LpMovable){
//        m_TrianglePos = glm::vec3(LinearInterpretMove(m_MoveData.startTime, 1.0, m_Time, m_MoveData.originPos, m_MoveData.targetPos, m_LpMovable) , 0.1);
////        m_CameraController.GetCamera().SetPosition(m_TrianglePos);
//    }

    Tso::Renderer2D::EndScene();

    m_FrameBuffer->Bind();
    Tso::Renderer2D::BeginScene(m_CameraController.GetCamera());

    for (float x = -5.0; x < 5.0; x += 0.5) {
        for (float y = -5.0; y < 5.0; y += 0.5) {
            glm::vec4 color = { (x + 5.0) / 10.0 ,0.4 , (y + 5.0) / 10.0 ,1.0 };
            Tso::Renderer2D::DrawQuad({ x , y }, { 0.45 , 0.45 }, color);
        }
    }


    Tso::Renderer2D::EndScene();
    m_FrameBuffer->UnBind();

    m_Time += ts.GetSecond();
    
}

void SandBox2D::OnEvent(Tso::Event& event)
{
    Tso::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Tso::MouseButtonPressedEvent>(BIND_EVENT_FN(SandBox2D::OnMouseButton));
    dispatcher.Dispatch<Tso::MouseMovedEvent>(BIND_EVENT_FN(SandBox2D::OnMouseMove));
    m_CameraController.OnEvent(event);

}

bool SandBox2D::OnMouseButton(Tso::MouseButtonPressedEvent& e){
    
    if(e.GetButton() == TSO_MOUSE_BUTTON_1){
        
        float width = 1280.f , height = 720.f;
        
        float asp = width / height;
        
        float targetX = m_MouseX / width * asp * 4 - 2 * asp;
        float targetY = m_MouseY / height * asp * 2 - asp;
        TSO_INFO("mouse position = ({0} , {1}) , and caled pos = ({2} , {3})" , m_MouseX , m_MouseY , targetX , targetY);

        
        m_MoveData.startTime = m_Time;
        m_MoveData.targetPos = glm::vec2(targetX , -targetY);
        m_MoveData.originPos = m_TrianglePos;
        m_LpMovable = true;
        return true;
    }
    return false;
}

bool SandBox2D::OnMouseMove(Tso::MouseMovedEvent& e){
    
    
    m_MouseX = e.GetX();
    m_MouseY = e.GetY();
    
    return false;
}

