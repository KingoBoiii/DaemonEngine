#pragma once
#include <box2d/box2d.h>

namespace Daemon
{

	class Physics2D
	{
	public:
		static void Initialize();
		static void Shutdown();
		static void Step();

		static void DebugDraw();

		static b2Body* CreateBody(b2BodyDef* bodyDef);
	};

}