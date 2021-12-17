#include "kepch.h"
#include "OpenGLPipeline.h"

#include <glad/glad.h>

namespace Daemon
{

	namespace Utils
	{
		GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::Float:    return GL_FLOAT;
				case ShaderDataType::Float2:   return GL_FLOAT;
				case ShaderDataType::Float3:   return GL_FLOAT;
				case ShaderDataType::Float4:   return GL_FLOAT;
				case ShaderDataType::Mat3:     return GL_FLOAT;
				case ShaderDataType::Mat4:     return GL_FLOAT;
				case ShaderDataType::Int:      return GL_INT;
				case ShaderDataType::Int2:     return GL_INT;
				case ShaderDataType::Int3:     return GL_INT;
				case ShaderDataType::Int4:     return GL_INT;
				case ShaderDataType::Bool:     return GL_BOOL;
			}
			//KE_CORE_ERROR(false, "Unknown ShaderDataType!");
			return 0;
		}
	}

	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& specification)
		: m_Specification(specification)
	{
		KE_CORE_ASSERT(specification.Shader);
		Invalidate();
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
		glDeleteVertexArrays(1, &m_VertexArrayId);
	}

	void OpenGLPipeline::Bind() const
	{
		m_Specification.Shader->Bind();
		glBindVertexArray(m_VertexArrayId);

		uint32_t attribIndex = 0;
		const BufferLayout& layout = m_Specification.InputLayout;
		for (const BufferElement& element : layout.GetElements())
		{
			auto glBaseType = Utils::ShaderDataTypeToOpenGLBaseType(element.Type);
			glEnableVertexAttribArray(attribIndex);
			if (glBaseType == GL_INT || glBaseType == GL_BOOL)
			{
				glVertexAttribIPointer(attribIndex, element.GetComponentCount(),glBaseType, layout.GetStride(), (const void*)(intptr_t)element.Offset);
			}
			else
			{
				GLboolean normalized = element.Normalized ? GL_TRUE : GL_FALSE;
				glVertexAttribPointer(attribIndex, element.GetComponentCount(), glBaseType, normalized, layout.GetStride(), (const void*)(intptr_t)element.Offset);
			}
			attribIndex++;
		}
	}

	void OpenGLPipeline::Invalidate()
	{
		KE_CORE_ASSERT(m_Specification.InputLayout.GetElements().size(), "Layout is empty!");

		if (m_VertexArrayId)
		{
			glDeleteVertexArrays(1, &m_VertexArrayId);
		}

		glGenVertexArrays(1, &m_VertexArrayId);
		glBindVertexArray(m_VertexArrayId);

#if 0
		uint32_t index = 0;
		const BufferLayout& layout = m_Specification.InputLayout;
		for (const BufferElement& element : layout.GetElements())
		{
			glEnableVertexAttribArray(index);
			GLenum type = Utils::ShaderDataTypeToOpenGLBaseType(element.Type);
			int componentSize = element.GetComponentCount();
			glVertexAttribPointer(index, componentSize, type, element.Normalized, layout.GetStride(), (const void*)element.Offset);

			index++;
		}
#endif

		glBindVertexArray(0);
	}

}