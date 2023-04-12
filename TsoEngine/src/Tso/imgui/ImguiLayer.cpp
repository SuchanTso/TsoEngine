//
//  ImguiLayer.cpp
//  TsoEngine
//
//  Created by user on 2023/4/12.
//

#include "TPch.h"
#include "ImguiLayer.h"
#include "imgui.h"
#include "Platform/OpenGL/ImGuiOpenglRenderer.h"
#include "GLFW/glfw3.h"
#include "Application.h"

namespace Tso{

#define BIND_EVENT_FN(x) std::bind(&ImGuiLayer::x,this , std::placeholders::_1)


ImGuiLayer::ImGuiLayer()
:Layer("ImGuiLayer"){
    
}

ImGuiLayer::~ImGuiLayer(){
    
}


void ImGuiLayer::OnUpdate(){
    
    ImGuiIO& io = ImGui::GetIO();
    
    Application& app = Application::Get();
    
    auto width = app.GetWindow().GetWidth();
    auto height = app.GetWindow().GetHeight();
    
    io.DisplaySize = ImVec2(app.GetWindow().GetWidth(),app.GetWindow().GetHeight());
    
    float time = (float)glfwGetTime();
    io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
    m_Time = time;
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    
    static bool show = true;
    ImGui::ShowDemoWindow(&show);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::OnEvent(Event &e){
    EventDispatcher dispatcher(e);
    
    dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(OnMouseMovedEvent));
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonPressedEvent));
    dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(OnMouseButtonReleasedEvent));
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OnMouseScrolledEvent));
    
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressedEvent));
    dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(OnKeyReleasedEvent));
    dispatcher.Dispatch<KeyTypedEvent>(BIND_EVENT_FN(OnKeyTypedEvent));
    
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResizedEvent));
    
}

void ImGuiLayer::OnAttach(){
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    //temp
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
    

    
    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::OnDetach(){
    
}

bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent &e){
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(e.GetX(),e.GetY());
    
    return false;
}

bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent &e){
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[e.GetButton()] = true;
    
    return false;
}

bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent &e){
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[e.GetButton()] = false;
    
    return false;
}

bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent &e){
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += e.GetXOffset();
    io.MouseWheelH+= e.GetYOffset();
    
    return false;
}

bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent &e){
    
    
    return  false;;
}

bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent &e){
    
    return false;
}

bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent &e){
    return false;

}

bool ImGuiLayer::OnWindowResizedEvent(WindowResizeEvent &e){
    return false;

}

}
