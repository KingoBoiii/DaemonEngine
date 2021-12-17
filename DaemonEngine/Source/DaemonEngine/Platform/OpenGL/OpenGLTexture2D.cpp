#include "kepch.h"
#include "OpenGLTexture2D.h"

#include <glad/glad.h>
#include <gl/GL.h>
#include "stb_image.h"

namespace Daemon
{

	namespace Utils
	{

		GLenum KingoTextureFilterToGL(TextureFilter filter)
		{
			switch (filter)
			{
				case TextureFilter::LINEAR:				return GL_LINEAR;
				case TextureFilter::NEAREST:			return GL_NEAREST;
				case TextureFilter::NONE:
				default: break;
			}
			return 0;
		}

		enum class FormatType { INTERNAL, DATA };
		GLenum KingoTextureFormatToGL(TextureFormat format, FormatType formatType)
		{
			switch (format)
			{
				case TextureFormat::RGBA:				return formatType == FormatType::DATA ? GL_RGBA : GL_RGBA8;
				case TextureFormat::RGB:				return formatType == FormatType::DATA ? GL_RGB : GL_RGB8;
				case TextureFormat::RED:				return formatType == FormatType::DATA ? GL_RED : GL_RED;	// 0x1909 = GL_LUMINANCE
				case TextureFormat::RED_INTEGER:		return formatType == FormatType::DATA ? GL_RED_INTEGER : GL_RED_INTEGER;	
				case TextureFormat::LUMINANCE_ALPHA:	return formatType == FormatType::DATA ? GL_RG : GL_RG;	// 0x190A = GL_LUMINANCE_ALPHA
				case TextureFormat::NONE:
				default: break;
			}
			return 0;
		}

		GLenum KingoTextureWrapToGL(TextureWrap wrap)
		{
			switch (wrap)
			{
				case TextureWrap::CLAMP:				return 0x2900;		// GL_CLAMP
				case TextureWrap::CLAMP_TO_BORDER:		return GL_CLAMP_TO_BORDER;
				case TextureWrap::CLAMP_TO_EDGE:		return GL_CLAMP_TO_EDGE;
				case TextureWrap::REPEAT:				return GL_REPEAT;
				case TextureWrap::MIRRORED_REPEAT:		return GL_MIRRORED_REPEAT;
				case TextureWrap::NONE:
				default: break;
			}
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath, const TextureSpecification& specification)
		: m_Width(1), m_Height(1), m_Specification(specification)
	{
		int32_t channels;
		stbi_set_flip_vertically_on_load(m_Specification.FlipVertically);
		unsigned char* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &channels, 0);
		if (data)
		{
			m_Specification.Format = channels == 4 ? TextureFormat::RGBA : TextureFormat::RGB;

			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTextureStorage2D(m_RendererID, 1, Utils::KingoTextureFormatToGL(m_Specification.Format, Utils::FormatType::INTERNAL), m_Width, m_Height);

			SetTextureParameters();

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, Utils::KingoTextureFormatToGL(m_Specification.Format, Utils::FormatType::DATA), GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSpecification& specification)
		: m_Width(width), m_Height(height), m_Specification(specification)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTextureStorage2D(m_RendererID, 1, Utils::KingoTextureFormatToGL(m_Specification.Format, Utils::FormatType::INTERNAL), m_Width, m_Height);

		SetTextureParameters();
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, void* data, const TextureSpecification& specification)
		: m_Width(width), m_Height(height), m_Specification(specification)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTextureStorage2D(m_RendererID, 1, Utils::KingoTextureFormatToGL(m_Specification.Format, Utils::FormatType::INTERNAL), m_Width, m_Height);

		SetTextureParameters();

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, Utils::KingoTextureFormatToGL(m_Specification.Format, Utils::FormatType::DATA), GL_UNSIGNED_BYTE, data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data) const
	{
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, Utils::KingoTextureFormatToGL(m_Specification.Format, Utils::FormatType::DATA), GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture2D::SetTextureParameters()
	{
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, Utils::KingoTextureFilterToGL(m_Specification.MinFilter));
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, Utils::KingoTextureFilterToGL(m_Specification.MagFilter));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, Utils::KingoTextureWrapToGL(m_Specification.WrapMode));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, Utils::KingoTextureWrapToGL(m_Specification.WrapMode));
	}

}