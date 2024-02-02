#pragma once
#include "Tso/Core/Core.h"

namespace Tso {

enum FrameBufferFormat{
    RGBA8 = 0,
    RGB8  = 1,
    DEPTH24_STENCIL8 = 2,
};


	struct FrameBufferInfo {
		uint32_t width = 720;
		uint32_t height = 1280;
        std::vector<FrameBufferFormat> format;
		bool isSwapChain = false;
	};

	class FrameBuffer {
	public:
		FrameBuffer(){}

		virtual ~FrameBuffer() {}

		static Ref<FrameBuffer> Create(const FrameBufferInfo& info);

		virtual void Bind() = 0;
		virtual void UnBind() = 0;
        virtual void Resize(const uint32_t& width , const uint32_t& height) = 0;

		virtual uint32_t GetColorAttachment(const uint32_t& index = 0)const = 0;
		virtual uint32_t GetDepthAttachment()const = 0;


	};
}
