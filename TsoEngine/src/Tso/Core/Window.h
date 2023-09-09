#pragma once
#include "TPch.h"

#include "Tso/Core/Core.h"
#include "Tso/Event/Event.h"

namespace Tso {
	struct WindowProps {
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string &title = "Tso Engine",
			unsigned int width = 1280,
			unsigned int height = 720
			): Title(title), Width(width), Height(height){}
	};

	class TSO_API Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window(){}

		virtual void OnUpdate() const = 0;

		virtual unsigned int GetWidth()const = 0;

		virtual unsigned int GetHeight()const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		virtual void SetVSync(bool enabled) = 0;

		static Window* Create(const WindowProps& props = WindowProps());

		virtual bool IsVSync()const = 0;

		virtual void* GetNativeWindow() const = 0;

	};

}