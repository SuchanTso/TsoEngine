#pragma once
#include "Tso/Core/Core.h"

namespace Tso {

enum FrameBufferFormat{
    
    RGBA8 = 1,
    RGB8  = 2,
    RED_INTEGER = 3,
    
    
    DEPTH24_STENCIL8 = 4,
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
        
        virtual int ReadOnePixel(const int& attachmentIndex , const int& x , const int& y) = 0;
        
        virtual void ClearAttachment(const int& attachmentIndex , const int& value) = 0;


	};
}
