#include "TPch.h"
#include "WindowsInput.h"
#include "GLFW/glfw3.h"
#include "Tso/Core/Application.h"
#include "utf8.h"

namespace Tso {

	Input* Input::s_Instance = new WindowsInput();

    struct InputData{
        std::string inputStringBuffer = "";
        std::unordered_map<int, int> lastMouseState;
        glm::vec2 viewportBoundMin = {0.f , 0.f};
        glm::vec2 viewportBoundMax = {1920.f , 1080.f};
    };
    static InputData s_Data;
    
    WindowsInput::WindowsInput(){
        

    }

    void WindowsInput::InitImpl(){
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        glfwSetCharCallback(window, CharCallback);
    }

    void WindowsInput::CharCallback(GLFWwindow* window, unsigned int codepoint) {
        try {
            utf8::append(codepoint, std::back_inserter(s_Data.inputStringBuffer));
        } catch (const std::exception& e) {
            // 错误处理
            TSO_CORE_ERROR("UTF-8 conversion error: {}", e.what());
        }
    }

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return  state == GLFW_PRESS;
	}

    bool WindowsInput::IsMouseButtonClickedImpl(int button){
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
            // 获取当前帧的状态
            int currentState = glfwGetMouseButton(window, button);
            // 获取上一帧的状态 (如果不存在，默认为 RELEASED)
            int lastState = GLFW_RELEASE;
            auto it = s_Data.lastMouseState.find(button);
            if (it != s_Data.lastMouseState.end()) {
                lastState = it->second;
            }

            return currentState == GLFW_PRESS && lastState == GLFW_RELEASE;
    }

    void WindowsInput::SetViewportBoundImpl(float Xmin , float Ymin , float Xmax , float Ymax){
        s_Data.viewportBoundMin = {Xmin , Ymin};
        s_Data.viewportBoundMax = {Xmax , Ymax};

    }

    std::string WindowsInput::GetTypedCharactersThisFrameImpl() {
        // 这个函数返回的是当前缓冲区的内容
        // 注意：我们在这里不清空，而是在 EndFrame 中清空
        return s_Data.inputStringBuffer;
    }


	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { (float)xpos,(float)ypos };
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return (float)xpos;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return (float)ypos;
	}

    float WindowsInput::GetViewportMouseXImpl(){
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        int xpos, ypos;
        glfwGetWindowPos(window, &xpos, &ypos);
        float screenX = GetMouseXImpl() + xpos;
        return screenX - s_Data.viewportBoundMin.x;
    }
    float WindowsInput::GetViewportMouseYImpl(){
        glm::vec2 viewportSize = s_Data.viewportBoundMax - s_Data.viewportBoundMin;
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        int xpos, ypos;
        glfwGetWindowPos(window, &xpos, &ypos);
    //    TSO_INFO("glfwWindow pos:[{} , {}]" , xpos , ypos);
    //    TSO_INFO("glfw mouse y {}" , GetMouseYImpl());
        float screenY = GetMouseYImpl() + ypos;
        return viewportSize.y - screenY + s_Data.viewportBoundMin.y;
    }

    void WindowsInput::EndFrameImpl(){
        s_Data.inputStringBuffer.clear();
        int buttonsToUpdate[] = {
            GLFW_MOUSE_BUTTON_LEFT,
            GLFW_MOUSE_BUTTON_RIGHT,
            GLFW_MOUSE_BUTTON_MIDDLE
        };
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        for (int button : buttonsToUpdate) {
            s_Data.lastMouseState[button] = glfwGetMouseButton(window, button);
        }
    }


}
