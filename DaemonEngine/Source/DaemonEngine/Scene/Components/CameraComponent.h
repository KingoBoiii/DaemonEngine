#pragma once
#include "DaemonEngine/Scene/SceneCamera.h"

namespace Daemon
{

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; 
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

}
