#pragma once

#include "Tso/Core/Core.h"

namespace Tso {

	class TSO_API Input {
	public:
        inline static void Init(){s_Instance->InitImpl();}

		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
        
        inline static bool IsMouseButtonClicked(int button){return s_Instance->IsMouseButtonClickedImpl(button);}

		inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }

		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }

		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
        
        inline static float GetViewportMouseX() { return s_Instance->GetViewportMouseXImpl(); }

        inline static float GetViewportMouseY() { return s_Instance->GetViewportMouseYImpl(); }
        
        inline static std::string GetTypedCharactersThisFrame(){return s_Instance->GetTypedCharactersThisFrameImpl();}
        
        inline static void EndFrame(){s_Instance->EndFrameImpl();}
        
        inline static void SetViewportBound(float Xmin , float Ymin , float Xmax , float Ymax){s_Instance->SetViewportBoundImpl(Xmin , Ymin , Xmax , Ymax);}
        


	protected:
        virtual void InitImpl() = 0;
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
        virtual bool IsMouseButtonClickedImpl(int button) = 0;

		virtual std::pair<float, float>GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
        
        virtual float GetViewportMouseXImpl() = 0;
        virtual float GetViewportMouseYImpl() = 0;

        virtual std::string GetTypedCharactersThisFrameImpl() = 0;
        virtual void EndFrameImpl() = 0;
        virtual void SetViewportBoundImpl(float Xmin , float Ymin , float Xmax , float Ymax) = 0;

	private:
		static Input* s_Instance;
	};
}
