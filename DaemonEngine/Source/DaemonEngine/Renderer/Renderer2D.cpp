#include "kepch.h"
#include "Renderer2D.h"
#include "Renderer.h"
#include "FontRenderer.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "Pipeline.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Daemon
{

#define MAX_QUADS			20000
#define MAX_VERTICES		MAX_QUADS * 4
#define MAX_INDICES			MAX_QUADS * 6
#define MAX_TEXTURE_SLOTS	32

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TextureIndex;

		// EDITOR ONLY
		int EntityId = 0;
	};

	struct Renderer2DData
	{
		Shared<Shader> QuadShader;
		Shared<Pipeline> QuadPipeline;
		Shared<VertexBuffer> QuadVertexBuffer;
		Shared<IndexBuffer> QuadIndexBuffer;
		Shared<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Shared<Texture2D>, MAX_TEXTURE_SLOTS> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ProjectionView;		// 64 bytes
			//uint8_t padding[192];			// 256 - 64 = 192 bytes - to fill out rest of the 256 bytes...
		};
		CameraData CameraBuffer;
		Shared<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData* s_Data = nullptr;

	void Renderer2D::Initialize()
	{
		s_Data = new Renderer2DData();

		PipelineSpecification quadPipelineSpec;
		memset(&quadPipelineSpec, 0, sizeof(PipelineSpecification));
		quadPipelineSpec.InputLayout = {
			{ ShaderDataType::Float3, "POSITION" },
			{ ShaderDataType::Float4, "COLOR" },
			{ ShaderDataType::Float2, "TEXCOORD" },
			{ ShaderDataType::Float, "TEXTUREID" },
			{ ShaderDataType::Int, "ENTITYID" }
		};
		quadPipelineSpec.Shader = Renderer::GetShaderLibrary()->Get("Renderer2D");
		quadPipelineSpec.Topology = PrimitiveTopology::Triangles;
		s_Data->QuadPipeline = Pipeline::Create(quadPipelineSpec);

		s_Data->QuadVertexBuffer = VertexBuffer::Create(MAX_VERTICES * sizeof(QuadVertex));

		uint32_t* quadIndices = new uint32_t[MAX_INDICES];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < MAX_INDICES; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		s_Data->QuadIndexBuffer = IndexBuffer::Create(quadIndices, MAX_INDICES);

		s_Data->QuadVertexBufferBase = new QuadVertex[MAX_VERTICES];

		s_Data->WhiteTexture = Renderer::GetWhiteTexture();

		int32_t samplers[MAX_TEXTURE_SLOTS];
		for (uint32_t i = 0; i < MAX_TEXTURE_SLOTS; i++)
			samplers[i] = i;

		// OpenGL stuff
		quadPipelineSpec.Shader->Bind();
		quadPipelineSpec.Shader->SetIntArray("u_Textures[0]", samplers, MAX_TEXTURE_SLOTS);

		s_Data->TextureSlots[0] = s_Data->WhiteTexture;

		s_Data->QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data->CameraUniformBuffer = UniformBuffer::Create((sizeof(Renderer2DData::CameraData) + 255) & ~255, 0);

		//FontRenderer::Initialize();
	}

	void Renderer2D::Shutdown()
	{
		//FontRenderer::Shutdown();

		delete s_Data;
	}

	void Renderer2D::BeginScene(const Camera& camera)
	{
		s_Data->CameraBuffer.ProjectionView = camera.GetProjView();
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
		ResetStats();
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_Data->CameraBuffer.ProjectionView = camera.GetProjection() * glm::inverse(transform);
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
		ResetStats();
	}

	void Renderer2D::EndScene()
	{
		if (s_Data->QuadIndexCount == 0)
		{
			return; // Nothing to draw
		}

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->QuadVertexBufferPtr - (uint8_t*)s_Data->QuadVertexBufferBase);
		s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertexBufferBase, dataSize);

		// Bind textures
		for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
		{
			s_Data->TextureSlots[i]->Bind(i);
		}

		Renderer::RenderGeometry(s_Data->QuadPipeline, s_Data->CameraUniformBuffer, s_Data->QuadVertexBuffer, s_Data->QuadIndexBuffer, s_Data->QuadIndexCount);
		s_Data->Stats.DrawCalls++;
	}

	// ---------------------------------
	// Default quads (rotation = 0)
	// ---------------------------------
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, color);
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) 
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Shared<Texture2D>& texture, const glm::vec4& tintColor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, texture, tintColor);
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Shared<Texture2D>& subtexture, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, subtexture, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Shared<SubTexture2D>& subtexture, const glm::vec4& tintColor)
	{
		const glm::vec2* coords = subtexture->GetTextureCoords();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, coords, subtexture, tintColor);
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Shared<SubTexture2D>& texture, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tintColor);
	}

	// ---------------------------------
	// Rotated quads
	// ---------------------------------
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, float rotation, const glm::vec2& size, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, rotation, size, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const Shared<Texture2D>& texture, const glm::vec4& tintColor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, texture, tintColor);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, float rotation, const glm::vec2& size, const Shared<Texture2D>& texture, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, rotation, size, texture, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const Shared<SubTexture2D>& subtexture, const glm::vec4& tintColor)
	{
		const glm::vec2* coords = subtexture->GetTextureCoords();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, coords, subtexture, tintColor);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, float rotation, const glm::vec2& size, const Shared<SubTexture2D>& subtexture, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, rotation, size, subtexture, tintColor);
	}

	// ---------------------------------
	// Base rendering of quads
	// ---------------------------------
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityId)
	{
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		float textureIndex = 0.0f;

		if (s_Data->QuadIndexCount >= MAX_INDICES)
		{
			NextBatch();
		}

		for (size_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[i];
			s_Data->QuadVertexBufferPtr->Color = color;
			s_Data->QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data->QuadVertexBufferPtr->TextureIndex = textureIndex;
			s_Data->QuadVertexBufferPtr->EntityId = entityId;
			s_Data->QuadVertexBufferPtr++;
		}

		s_Data->QuadIndexCount += 6;
		s_Data->Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Shared<Texture2D>& texture, const glm::vec4& tintColor, int entityId)
	{
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		float textureIndex = BindTexture(texture);

		if (s_Data->QuadIndexCount >= MAX_INDICES)
		{
			NextBatch();
		}

		for (size_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[i];
			s_Data->QuadVertexBufferPtr->Color = tintColor;
			s_Data->QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data->QuadVertexBufferPtr->TextureIndex = textureIndex;
			s_Data->QuadVertexBufferPtr->EntityId = entityId;
			s_Data->QuadVertexBufferPtr++;
		}

		s_Data->QuadIndexCount += 6;
		s_Data->Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec2* texCoords, const Shared<SubTexture2D>& subtexture, const glm::vec4& tintColor, int entityId)
	{
		float textureIndex = BindTexture(subtexture->GetTexture());

		if (s_Data->QuadIndexCount >= MAX_INDICES)
		{
			NextBatch();
		}

		for (size_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[i];
			s_Data->QuadVertexBufferPtr->Color = tintColor;
			s_Data->QuadVertexBufferPtr->TexCoord = texCoords[i];
			s_Data->QuadVertexBufferPtr->TextureIndex = textureIndex;
			s_Data->QuadVertexBufferPtr->EntityId = entityId;
			s_Data->QuadVertexBufferPtr++;
		}

		s_Data->QuadIndexCount += 6;
		s_Data->Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness, int entityId)
	{
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_Data->QuadIndexCount >= MAX_INDICES)
		{
			NextBatch();
		}

		glm::vec2 normal = glm::normalize(glm::vec2(end.y - start.y, -(end.x - start.x))) * thickness;
		glm::vec3 vertex1 = glm::vec3(start.x + normal.x, start.y + normal.y, 0.0f);
		glm::vec3 vertex2 = glm::vec3(end.x +	normal.x, end.y +	normal.y, 0.0f);
		glm::vec3 vertex3 = glm::vec3(end.x -	normal.x, end.y -	normal.y, 0.0f);
		glm::vec3 vertex4 = glm::vec3(start.x -	normal.x, start.y -	normal.y, 0.0f);

		for (size_t i = 0; i < 4; i++)
		{
			glm::vec3 vertex;
			if (i == 0) vertex = vertex1;
			if (i == 1) vertex = vertex2;
			if (i == 2) vertex = vertex3;
			if (i == 3) vertex = vertex4;

			s_Data->QuadVertexBufferPtr->Position = vertex;
			s_Data->QuadVertexBufferPtr->Color = color;
			s_Data->QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data->QuadVertexBufferPtr->TextureIndex = 0.0f;
			s_Data->QuadVertexBufferPtr->EntityId = entityId;
			s_Data->QuadVertexBufferPtr++;
		}

		s_Data->QuadIndexCount += 6;
		s_Data->Stats.QuadCount++;
	}

	void Renderer2D::DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, int entityId)
	{
		DrawLine({ position.x, position.y },					{ position.x + size.x, position.y },			color, entityId);
		DrawLine({ position.x + size.x, position.y },			{ position.x + size.x, position.y + size.y },	color, entityId);
		DrawLine({ position.x + size.x, position.y + size.y },	{ position.x, position.y + size.y},				color, entityId);
		DrawLine({ position.x, position.y + size.y },			{ position.x, position.y },						color, entityId);
	}

	void Renderer2D::DrawCircle(const glm::vec2& position, float radius, const glm::vec4& color, float thickness, int entityId)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position.x, position.y, 0.0f }) * glm::scale(glm::mat4(1.0f), glm::vec3(radius));

		int segments = 32;
		for (int i = 0; i < segments; i++)
		{
			float angle = 2.0f * glm::pi<float>() * (float)i / segments;
			glm::vec4 startPosition = { glm::cos(angle), glm::sin(angle), 0.0f, 1.0f };
			angle = 2.0f * glm::pi<float>() * (float)((i + 1) % segments) / segments;
			glm::vec4 endPosition = { glm::cos(angle), glm::sin(angle), 0.0f, 1.0f };

			glm::vec3 p0 = transform * startPosition;
			glm::vec3 p1 = transform * endPosition;
			DrawLine(p0, p1, color, thickness, entityId);
		}
	}

	void Renderer2D::DrawSprite(TransformComponent& transform, SpriteRendererComponent& src, int entityId)
	{
		switch (src.Type)
		{
			case SpriteRendererComponent::PolygonType::Rect:
			{
				RectData* data = static_cast<RectData*>(src.Data);
				KE_CORE_ASSERT(data, "Invalid rect data");
				glm::vec2 rectPos = { transform.Position.x, transform.Position.y };
				Renderer2D::DrawRect(rectPos, data->Size, data->Color, entityId);
			} break;
			case SpriteRendererComponent::PolygonType::Line:
			{
				LineData* data = static_cast<LineData*>(src.Data);
				KE_CORE_ASSERT(data, "Invalid line data");
				Renderer2D::DrawLine(data->StartPosition, data->EndPosition, data->Color, entityId);
			} break;
			case SpriteRendererComponent::PolygonType::Circle:
			{
				CircleData* data = static_cast<CircleData*>(src.Data);
				KE_CORE_ASSERT(data, "Invalid circle data");
				glm::vec2 circlePos = { transform.Position.x, transform.Position.y };
				Renderer2D::DrawCircle(circlePos, data->Radius, data->Color, entityId);
			} break;
			case SpriteRendererComponent::PolygonType::Quad:
			default:
			{
				QuadData* data = static_cast<QuadData*>(src.Data);
				KE_CORE_ASSERT(data, "Invalid quad data");

				if (data && data->Sprite)
				{
					Renderer2D::DrawQuad(transform.GetTransform(), data->Sprite, data->Color, entityId);
				}
				else if (data->SubSprite)
				{
					const glm::vec2* texCoords = data->SubSprite->GetTextureCoords();
					Renderer2D::DrawQuad(transform.GetTransform(), texCoords, data->SubSprite, data->Color, entityId);
				}
				else
				{
					Renderer2D::DrawQuad(transform.GetTransform(), data->Color, entityId);
				}
			} break;
		}
	}

	void Renderer2D::StartBatch()
	{
		s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;
		s_Data->QuadIndexCount = 0;
		s_Data->TextureSlotIndex = 1;
	}

	void Renderer2D::NextBatch()
	{
		EndScene();
		StartBatch();
	}

	float Renderer2D::BindTexture(const Shared<Texture2D>& texture)
	{
		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (*s_Data->TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f && texture)
		{
			if (s_Data->TextureSlotIndex >= MAX_TEXTURE_SLOTS)
			{
				NextBatch();
			}

			textureIndex = (float)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
			s_Data->TextureSlotIndex++;
		}
		return textureIndex;
	}

	const Renderer2D::Statistics& Renderer2D::GetStats()
	{
		return s_Data->Stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data->Stats, 0, sizeof(Renderer2D::Statistics));
	}

}