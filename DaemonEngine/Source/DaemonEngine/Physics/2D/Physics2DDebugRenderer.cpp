#include "kepch.h"
#include "Physics2DDebugRenderer.h"

#include "DaemonEngine/Renderer/Renderer2D.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Daemon
{

	void Physics2DDebugRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
		float lbX = vertices[0].x;
		float lbY = vertices[0].y;
		float ubX = vertices[2].x;
		float ubY = vertices[2].y;

		Renderer2D::DrawLine({ lbX, lbY }, { ubX, lbY }, { color.r, color.g, color.b, color.a });
		Renderer2D::DrawLine({ ubX, lbY }, { ubX, ubY }, { color.r, color.g, color.b, color.a });
		Renderer2D::DrawLine({ ubX, ubY }, { lbX, ubY }, { color.r, color.g, color.b, color.a });
		Renderer2D::DrawLine({ lbX, ubY }, { lbX, lbY }, { color.r, color.g, color.b, color.a });
	}

	void Physics2DDebugRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
	}

	void Physics2DDebugRenderer::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
	{
		Renderer2D::DrawCircle({ center.x, center.y }, radius, { color.r, color.g, color.b, color.a });
	}

	void Physics2DDebugRenderer::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
	{
	}

	void Physics2DDebugRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
	{
	}

	void Physics2DDebugRenderer::DrawTransform(const b2Transform& xf)
	{
	}

	void Physics2DDebugRenderer::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
	{
	}

}