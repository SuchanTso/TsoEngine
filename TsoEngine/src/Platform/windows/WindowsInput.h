#pragma once

#include "Tso/Core/Input.h"
class GLFWwindow;
namespace Tso {
	class WindowsInput : public Input
	{
    public:
        WindowsInput();

	protected:
        virtual void InitImpl()override;
		virtual bool IsKeyPressedImpl(int keycode)override;

		virtual bool IsMouseButtonPressedImpl(int button)override;
        virtual bool IsMouseButtonClickedImpl(int button)override;

		virtual std::pair<float, float> GetMousePositionImpl()override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
        virtual std::string GetTypedCharactersThisFrameImpl()override;
        virtual void EndFrameImpl()override;
        virtual void SetViewportBoundImpl(float Xmin , float Ymin , float Xmax , float Ymax)override;
        virtual float GetViewportMouseXImpl()override;
        virtual float GetViewportMouseYImpl()override;

    private:
        static void CharCallback(GLFWwindow* window, unsigned int codepoint);
	};


}
