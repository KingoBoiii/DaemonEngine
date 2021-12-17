#pragma once

namespace Daemon
{

	enum class TextureWrap
	{
		NONE = 0,
		REPEAT,
		CLAMP,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER
	};

	enum class TextureFilter
	{
		NONE = 0,
		LINEAR,
		NEAREST
	};

	enum class TextureFormat
	{
		NONE = 0,
		RGB,
		RGBA,
		RED,
		RED_INTEGER,
		LUMINANCE_ALPHA
	};

	struct TextureSpecification
	{
		TextureFormat Format;
		TextureFilter Filter;
		TextureFilter MinFilter;
		TextureFilter MagFilter;
		TextureWrap WrapMode;
		bool FlipVertically = true;

		TextureSpecification()
		{
			Format = TextureFormat::RGBA;
			Filter = TextureFilter::LINEAR;

			MinFilter = TextureFilter::LINEAR;
			MagFilter = TextureFilter::NEAREST;

			WrapMode = TextureWrap::REPEAT;
		}
	};

	class Texture2D
	{
	public:
		static Shared<Texture2D> Create(const std::string& filepath, const TextureSpecification& specification = TextureSpecification());
		static Shared<Texture2D> Create(uint32_t width, uint32_t height, const TextureSpecification& specification = TextureSpecification());
		static Shared<Texture2D> Create(uint32_t width, uint32_t height, void* data, const TextureSpecification& specification = TextureSpecification());
	public:
		virtual ~Texture2D() = default;

		virtual void SetData(void* data) const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind() const = 0;

		virtual void* GetTexture() const = 0;

		virtual bool operator==(const Texture2D& other) const = 0;
	};

}
