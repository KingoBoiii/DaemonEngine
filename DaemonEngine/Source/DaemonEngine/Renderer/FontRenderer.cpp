#include "kepch.h"
#include "FontRenderer.h"
#include "Renderer.h"
#include "Shader.h"

#include <ft2build.h>
#include <freetype.h>
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

/*
	* TODO:
	* Remove RAW OpenGL calls
	* Optimize the font renderer
	* Maybe move it to Renderer2D
*/

namespace Daemon
{

	struct Character
	{
		unsigned int TextureID;  // ID handle of the glyph texture
		glm::ivec2   Size;       // Size of glyph
		glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
		unsigned int Advance;    // Offset to advance to next glyph
	};

	std::map<char, Character> Characters;

	struct FontRendererData
	{
		Shared<Shader> FontShader;
		uint32_t VAO;
		uint32_t VBO;
	};

	static FontRendererData* s_Data = nullptr;

	void FontRenderer::Initialize()
	{
		s_Data = new FontRendererData();

		s_Data->FontShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Text");
		SetupTextRendering();
	}

	void FontRenderer::Shutdown()
	{
		delete s_Data;
	}

	void FontRenderer::DrawString(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color)
	{
		glm::mat4 ortho = glm::ortho(0.0f, 800.0f, 0.0f, 800.0f);

		s_Data->FontShader->Bind();
		s_Data->FontShader->SetFloat3("textColor", color);
		s_Data->FontShader->SetMat4("projection", ortho);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(s_Data->VAO);

		float x = position.x;

		// iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];

			float xpos = x + ch.Bearing.x * scale;
			float ypos = position.y - (ch.Size.y - ch.Bearing.y) * scale;

			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;
			// update VBO for each character
			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};
			// render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, s_Data->VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void FontRenderer::SetupTextRendering()
	{
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			KE_CORE_ERROR("Failed to initialize FreeType!");
			return;
		}

		FT_Face face;
		if (FT_New_Face(ft, "Resources/Fonts/arial.ttf", 0, &face))
		{
			KE_CORE_ERROR("Failed to load font!");
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, 48);

		if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
		{
			KE_CORE_ERROR("Failed to load Glyph");
			return;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

		for (unsigned char c = 0; c < 128; c++)
		{
			// load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		glGenVertexArrays(1, &s_Data->VAO);
		glGenBuffers(1, &s_Data->VBO);
		glBindVertexArray(s_Data->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

}
