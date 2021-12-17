#pragma once
#include "DaemonEngine/Renderer/Pipeline.h"

namespace Daemon
{

	class OpenGLPipeline : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& specification);
		virtual ~OpenGLPipeline();

		virtual void Bind() const override;

		virtual PipelineSpecification GetSpecification() const override { return m_Specification; }
		virtual PipelineSpecification GetSpecification() override { return m_Specification; }
	private:
		void Invalidate();
	private:
		PipelineSpecification m_Specification;
		uint32_t m_VertexArrayId;
	};

}
