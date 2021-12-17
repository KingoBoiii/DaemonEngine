#pragma once
#include "Shader.h"
#include "BufferLayout.h"

namespace Daemon
{

	enum class PrimitiveTopology
	{
		None,
		Points,
		Lines,
		Triangles
	};

	struct PipelineSpecification
	{
		Shared<Shader> Shader;
		BufferLayout InputLayout;
		PrimitiveTopology Topology = PrimitiveTopology::Triangles;
		bool Wireframe = false;

		PipelineSpecification() = default;
	};

	class Pipeline
	{
	public:
		static Shared<Pipeline> Create(const PipelineSpecification& specification = PipelineSpecification());
	public:
		virtual ~Pipeline() = default;

		virtual void Bind() const = 0;

		virtual PipelineSpecification GetSpecification() const = 0;
		virtual PipelineSpecification GetSpecification() = 0;
	};

}
