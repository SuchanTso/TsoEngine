#pragma once

namespace Tso {

	enum class ShaderDataType {
		None = 0,
		Float ,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int ,
		Int2,
		Int3,
		Int4,
		Bool,
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type) {
		switch (type)
		{
		case ShaderDataType::Float:		return sizeof(float);
		case ShaderDataType::Float2:	return sizeof(float) * 2;
		case ShaderDataType::Float3:	return sizeof(float) * 3;
		case ShaderDataType::Float4:	return sizeof(float) * 4;
		case ShaderDataType::Mat3:		return sizeof(float) * 3 * 3;
		case ShaderDataType::Mat4:		return sizeof(float) * 4 * 4;
		case ShaderDataType::Int:		return sizeof(int);
		case ShaderDataType::Int2:		return sizeof(int) * 2;
		case ShaderDataType::Int3:		return sizeof(int) * 3;
		case ShaderDataType::Int4:		return sizeof(int) * 4;
		case ShaderDataType::Bool:		return 1;
		default:
			break;
		}
		TSO_CORE_ASSERT(false, "Unknown ShaderDataType chosen here!");
		return 0;
		
	}


	struct BufferElement {
		std::string Name;
		ShaderDataType Type;

		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement( const ShaderDataType type , const std::string name , bool normalized = false)
		:Name(name) , Type(type) , Size(ShaderDataTypeSize(type)) , Offset(0) , Normalized(normalized){
		
		}


		const uint32_t GetComponentCount() const{
			switch (Type)
			{
			case Tso::ShaderDataType::None:		return 0;
			case Tso::ShaderDataType::Float:	return 1;
			case Tso::ShaderDataType::Float2:	return 2;
			case Tso::ShaderDataType::Float3:	return 3;
			case Tso::ShaderDataType::Float4:	return 4;
			case Tso::ShaderDataType::Mat3:		return 3 * 3;
			case Tso::ShaderDataType::Mat4:		return 4 * 4;
			case Tso::ShaderDataType::Int:		return 1;
			case Tso::ShaderDataType::Int2:		return 2;
			case Tso::ShaderDataType::Int3:		return 3;
			case Tso::ShaderDataType::Int4:		return 4;
			case Tso::ShaderDataType::Bool:		return 1;

			}

			TSO_CORE_ASSERT(false, "Unknown ShaderDataType here!");
			return 0;
		}
	};


	class BufferLayout {
	public: 
		BufferLayout(){}
		BufferLayout(const std::initializer_list<BufferElement>& elements)
		:m_BufferElement(elements){
			CalculateOffsetAndStride();
		}

		inline const std::vector<BufferElement>& GetElements()const { return m_BufferElement; }
		inline const uint32_t GetStride()const { return m_Stride; }

	private:
		void CalculateOffsetAndStride() {
			
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_BufferElement) {
				element.Offset = offset;
				offset += element.Size;
			}
			m_Stride = offset;

		}
	public:
		std::vector<BufferElement>::iterator begin() { return m_BufferElement.begin(); }
		std::vector<BufferElement>::iterator end() { return m_BufferElement.end(); }

		std::vector<BufferElement>::const_iterator begin()const { return m_BufferElement.begin(); }
		std::vector<BufferElement>::const_iterator end()const { return m_BufferElement.end(); }
	private:
		std::vector<BufferElement> m_BufferElement;
		uint32_t m_Stride = 0;
		uint32_t m_Offset = 0;


		
	};

	class VertexBuffer {
	public:

		virtual ~VertexBuffer(){}

		virtual void Bind() const  = 0;
		virtual void UnBind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout()const = 0;

		static VertexBuffer* Create(float* vertices, uint32_t size);
		static VertexBuffer* Create(uint32_t size);

	};

	class IndexBuffer {
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual uint32_t GetCount() = 0;

		static IndexBuffer* Create(uint32_t* indices, uint32_t count);

	};
}