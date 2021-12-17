#pragma once
#include <glm/glm.hpp>

namespace Daemon
{

	class Math
	{
	public:
		static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outPosition, glm::vec3& outRotation, glm::vec3& outScale);
		static void InvertYComponent(glm::mat4& matrix);

		// Generate a 1D Perlin Noise, useful for 2D platformer terrain generation
		static void PerlineNoise1D(int32_t count, int32_t seed, int32_t octaves, float bias, float* output);
		// Generate a 1D Perlin Noise, useful for 2D platformer terrain generation
		static void PerlineNoise1D(int32_t count, float* seed, int32_t octaves, float bias, float* output);

		static float RadiansToDegrees(float radians);
		static float DegreesToRadians(float degrees);
	};

}
