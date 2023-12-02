#include "EditorLayer.h"
#include "imgui.h"



namespace Tso {
    EditorLayer::EditorLayer()
        :Layer("EditorLayer"),
        m_CameraController(1280.0 / 720, true),
        m_TrianglePos(glm::vec3(0.f))
    {
        m_ShaderLibrary = std::make_shared<ShaderLibrary>();

        m_Shader = m_ShaderLibrary->Load("asset/shader/Shader2D.glsl");
        Renderer2D::Init(m_Shader);

        std::string lp = "asset/lp2.png";

        std::string tileMap = "asset/tilemap_packed.png";


        m_Texture = Texture2D::Create(lp);
        m_TileTexture = Texture2D::Create(tileMap);
        m_subTexture = SubTexture2D::CreateByCoord(m_TileTexture, { 16.0 , 16.0 }, { 2.0 , 3.0 }, { 1.0 , 1.0 });
        m_sub1 = SubTexture2D::CreateByCoord(m_TileTexture, { 16.0 , 16.0 }, { 6.0 , 0.0 }, { 1.0 , 1.0 });

        FrameBufferInfo info = { (uint32_t)m_ViewportSize.x , (uint32_t)m_ViewportSize.y , false };
        m_FrameBuffer = FrameBuffer::Create(info);

        m_TrianglePos = glm::vec3(0.0, 0.0, 0.1);

        m_MoveData.startTime = m_Time;
        m_MoveData.targetPos = glm::vec2(0.0, 0.0);
        m_MoveData.originPos = glm::vec2(0.0, 0.0);
    }


    void EditorLayer::OnImGuiRender()
    {

    }


    template<typename T>
    T EditorLayer::LinearInterpretMove(const float& start, const float& duration, const float& timestamp, const T& pos, const T& target, bool& movable) {
        if (timestamp > start + duration) {
            movable = false;
            return target;
        }
        if (!movable) {
            return pos;
        }
        float ratio = (timestamp - start) / duration;
        T res = (target - pos) * ratio + pos;
        //    TSO_INFO("moving , start from {0} , duration is {1} , time = {2}" , start , duration , timestamp);

        return res;
    }


    void EditorLayer::OnUpdate(TimeStep ts)
    {

        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
        RenderCommand::Clear();

        if (Input::IsKeyPressed(TSO_KEY_I)) {
            m_TrianglePos.y += m_MoveSpeed * ts;
            TSO_INFO("moved Triangle at {0}", m_TrianglePos.y);

        }
        if (Input::IsKeyPressed(TSO_KEY_K)) {
            m_TrianglePos.y -= m_MoveSpeed * ts;
            TSO_INFO("moved Triangle at {0}", m_TrianglePos.y);

        }
        if (Input::IsKeyPressed(TSO_KEY_J)) {
            m_TrianglePos.x -= m_MoveSpeed * ts;
            TSO_INFO("moved Triangle at {0}", m_TrianglePos.x);

        }
        if (Input::IsKeyPressed(TSO_KEY_L)) {
            m_TrianglePos.x += m_MoveSpeed * ts;
            TSO_INFO("moved Triangle at {0}", m_TrianglePos.x);
        }
        if (Input::IsKeyPressed(TSO_KEY_SPACE)) {
            m_MoveData.startTime = m_Time;
            m_MoveData.targetPos = glm::vec2(0.5, 0.5);
            m_MoveData.originPos = m_TrianglePos;
            m_LpMovable = true;
        }

        Renderer2D::ResetStat();

        m_CameraController.OnUpdate(ts);
        Renderer2D::BeginScene(m_CameraController.GetCamera());
        /* Renderer2D::DrawQuad({-0.5 , 0.0 , 0.0}, 45.f, {0.5 , 0.5}, {0.8 , 0.3 , 0.2 , 1.0});
         Renderer2D::DrawQuad({0.0 , 0.0 , 0.0}, 0.f, {1.0 , 1.0}, {0.2 , 0.8 , 0.3 , 1.0});*/
        Renderer2D::DrawQuad({ 0.5 , 0.5 , 0.0 }, 0.f, { 0.5 , 0.5 }, m_TileTexture);
        Renderer2D::DrawQuad({ 1.5 , 0.5 , 0.0 }, 0.f, { 0.5 , 0.5 }, m_subTexture);
        Renderer2D::DrawQuad({ 1.5 , -0.5 , 0.0 }, 0.f, { 0.5 , 0.5 }, m_sub1);


        Renderer2D::EndScene();

        m_FrameBuffer->Bind();
        RenderCommand::Clear();
        Renderer2D::BeginScene(m_CameraController.GetCamera());
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
        RenderCommand::Clear();

        for (float x = -5.0; x < 5.0; x += 0.5) {
            for (float y = -5.0; y < 5.0; y += 0.5) {
                glm::vec4 color = { (x + 5.0) / 10.0 ,0.4 , (y + 5.0) / 10.0 ,1.0 };
                Renderer2D::DrawQuad({ x , y }, { 0.45 , 0.45 }, color);
            }
        }


        Renderer2D::EndScene();
        m_FrameBuffer->UnBind();

        m_Time += ts.GetSecond();

    }

    void EditorLayer::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButton));
        dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseMove));
        m_CameraController.OnEvent(event);

    }

    bool EditorLayer::OnMouseButton(MouseButtonPressedEvent& e) {

        if (e.GetButton() == TSO_MOUSE_BUTTON_1) {

            float width = 1280.f, height = 720.f;

            float asp = width / height;

            float targetX = m_MouseX / width * asp * 4 - 2 * asp;
            float targetY = m_MouseY / height * asp * 2 - asp;
            TSO_INFO("mouse position = ({0} , {1}) , and caled pos = ({2} , {3})", m_MouseX, m_MouseY, targetX, targetY);


            m_MoveData.startTime = m_Time;
            m_MoveData.targetPos = glm::vec2(targetX, -targetY);
            m_MoveData.originPos = m_TrianglePos;
            m_LpMovable = true;
            return true;
        }
        return false;
    }

    bool EditorLayer::OnMouseMove(MouseMovedEvent& e) {


        m_MouseX = e.GetX();
        m_MouseY = e.GetY();

        return false;
    }

}