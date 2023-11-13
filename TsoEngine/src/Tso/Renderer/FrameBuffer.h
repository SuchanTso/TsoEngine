#pragma once
#include "Tso/Core/Core.h"

namespace Tso {

	struct FrameBufferInfo {
		uint32_t width = 720;
		uint32_t height = 1280;

		bool isSwapChain = false;
	};

	class FrameBuffer {
	public:
		FrameBuffer(){}

		virtual ~FrameBuffer() {}

		static Ref<FrameBuffer> Create(const FrameBufferInfo& info);

		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		virtual uint32_t GetColorAttachment()const = 0;
		virtual uint32_t GetDepthAttachment()const = 0;


	};
}