#include "kepch.h"
#include "Math.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Daemon
{
	bool Math::DecomposeTransform(const glm::mat4& transform, glm::vec3& outPosition, glm::vec3& outRotation, glm::vec3& outScale)
	{
		using namespace glm;
		using T = float;

		mat4 localMatrix(transform);

		if (epsilonEqual(localMatrix[3][3], static_cast<float>(0), epsilon<T>()))
		{
			return false;
		}

		if (
			epsilonEqual(localMatrix[0][3], static_cast<float>(0), epsilon<T>()) ||
			epsilonEqual(localMatrix[1][3], static_cast<float>(0), epsilon<T>()) ||
			epsilonEqual(localMatrix[2][3], static_cast<float>(0), epsilon<T>())
			)
		{
			localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = static_cast<T>(0);
			localMatrix[3][3] = static_cast<T>(1);
		}

		//outPosition = vec3(localMatrix[3]);
		outPosition = vec3(-localMatrix[3]);
		//outPosition = vec3(-localMatrix[3].x, -localMatrix[3].y, -localMatrix[3].z);
		localMatrix[3] = vec4(0, 0, 0, localMatrix[3].w);

		vec3 Row[3], Pdum3;
		for (length_t i = 0; i < 3; i++)
		{
			for (length_t j = 0; j < 3; j++)
			{
				Row[i][j] = localMatrix[i][j];
			}
		}

		outScale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		outScale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		outScale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

#if 0
		Pdum3 = cross(Row[1], Row[2]);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				outScale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		outRotation.y = asin(-Row[0][2]);
		if (cos(outRotation.y) != 0)
		{
			outRotation.x = atan2(Row[1][2], Row[2][2]);
			outRotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else
		{
			outRotation.x = atan2(Row[2][0], Row[1][1]);
			outRotation.z = 0;
		}

		return true;
	}

	void Math::InvertYComponent(glm::mat4& matrix)
	{
		matrix[1][1] = -matrix[1][1];
	}

	void Math::PerlineNoise1D(int32_t count, int32_t seed, int32_t octaves, float bias, float* output)
	{
		constexpr int32_t width = 128;
		float* noiseSeed1D = new float[width];

		srand(seed);
		for (int32_t i = 0; i < width; i++)
		{
			for (int i = 0; i < width; i++) noiseSeed1D[i] = (float)rand() / (float)RAND_MAX;
		}
		PerlineNoise1D(count, noiseSeed1D, octaves, bias, output);
	}

	void Math::PerlineNoise1D(int32_t count, float* seed, int32_t octaves, float bias, float* output)
	{
		for (int32_t x = 0; x < count; x++)
		{
			float noise = 0.0f;
			float scaleAcc = 0.0f;
			float scale = 1.0f;

			for (int32_t o = 0; o < octaves; o++)
			{
				int32_t pitch = count >> o;
				int sample1 = (x / pitch) * pitch;
				int sample2 = (sample1 + pitch) % count;

				float blend = (float)(x - sample1) / (float)pitch;
				float sample = (1.0f - blend) * seed[sample1] + blend * seed[sample2];

				scaleAcc += scale,
					noise += sample * scale;
				scale = scale / bias;
			}
			output[x] = noise / scaleAcc;
		}
	}

	float Math::RadiansToDegrees(float radians)
	{
		float pi = static_cast<float>(M_PI);
		return radians * (180.0f / pi);
	}

	float Math::DegreesToRadians(float degrees)
	{
		float pi = static_cast<float>(M_PI);
		return degrees * (pi / 180.0f);
	}

}
