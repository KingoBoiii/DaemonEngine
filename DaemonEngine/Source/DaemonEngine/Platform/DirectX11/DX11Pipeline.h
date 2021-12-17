#pragma once
#include "DaemonEngine/Renderer/Pipeline.h"

#include <d3d11.h>

namespace Daemon
{

	class DX11Pipeline : public Pipeline
	{
	public:
		DX11Pipeline(const PipelineSpecification& specification);
		virtual ~DX11Pipeline();

		virtual void Bind() const override;

		virtual PipelineSpecification GetSpecification() const override { return m_Specification; }
		virtual PipelineSpecification GetSpecification() override { return m_Specification; }
	private:
		void Invalidate();
		void CreateRasterizerState();
	private:
		PipelineSpecification m_Specification;

		ID3D11InputLayout* m_InputLayout;
		ID3D11RasterizerState* m_RasterizerState;
	};

}