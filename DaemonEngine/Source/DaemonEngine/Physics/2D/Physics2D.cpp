#include "kepch.h"
#include "Physics2D.h"

#include "Physics2DDebugRenderer.h"

namespace Daemon
{

	struct Physics2DData
	{
		Unique<b2World> World;
		b2Vec2 Gravity = { 0.0f, -9.8f };

		float FixedDeltaTime = 1.0f / 60.0f;
		int32_t VelocityIterations = 6;
		int32_t PositionIterations = 2;
	};
	static Physics2DData* s_Data = nullptr;

	void Physics2D::Initialize()
	{
		s_Data = new Physics2DData();

		s_Data->World = CreateUnique<b2World>(s_Data->Gravity);

		Physics2DDebugRenderer* debugRenderer = new Physics2DDebugRenderer();
		debugRenderer->SetFlags(b2Draw::e_shapeBit | b2Draw::e_aabbBit | b2Draw::e_centerOfMassBit | b2Draw::e_pairBit | b2Draw::e_jointBit);
		s_Data->World->SetDebugDraw(debugRenderer);
	}

	void Physics2D::Shutdown()
	{
		//delete s_Data;
	}

	void Physics2D::Step()
	{
		s_Data->World->Step(s_Data->FixedDeltaTime, s_Data->VelocityIterations, s_Data->PositionIterations);
	}

	void Physics2D::DebugDraw()
	{
		s_Data->World->DebugDraw();
	}

	b2Body* Physics2D::CreateBody(b2BodyDef* bodyDef)
	{
		return s_Data->World->CreateBody(bodyDef);
	}

}
