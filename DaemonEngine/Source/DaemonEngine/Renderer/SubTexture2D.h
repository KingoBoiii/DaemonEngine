#pragma once
#include "Texture2D.h"

#include <glm/glm.hpp>

namespace Daemon
{

	class SubTexture2D
	{
	public:
		static Shared<SubTexture2D> CreateFromCoords(const Shared<Texture2D> texture, const glm::vec2& coords, const glm::vec2& spriteSize);
	public:
		SubTexture2D(const Shared<Texture2D> texture, const glm::vec2& min, const glm::vec2& max);

		const Shared<Texture2D>& GetTexture() const { return m_Texture; }
		const glm::vec2* GetTextureCoords() const { return m_TexCoord; }
	private:
		Shared<Texture2D> m_Texture;
		glm::vec2 m_TexCoord[4];
	};

}
