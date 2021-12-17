#include "kepch.h"
#include "DX11UniformBuffer.h"
#include "DX11Context.h"
//#include "DX11Shader.h"

namespace Daemon
{

	DX11UniformBuffer::DX11UniformBuffer(uint32_t size, uint32_t binding)
		: m_Binding(binding)
	{
		// Fill in a buffer description.
		D3D11_BUFFER_DESC constantBufferDesc;
		constantBufferDesc.ByteWidth = size;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;
		constantBufferDesc.StructureByteStride = 0;

		DX11Context& context = DX11Context::Get();
		HRESULT result = context.GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_CBuffer);
		KE_CORE_ASSERT(!FAILED(result) && m_CBuffer, "Failed to create Constant Buffer!");
	}

	DX11UniformBuffer::~DX11UniformBuffer()
	{
		ReleaseCOM(m_CBuffer);
	}

	void DX11UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		memset(&msr, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

		DX11Context& context = DX11Context::Get();
		context.GetDeviceContext()->Map(m_CBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msr);
		memcpy(msr.pData, data, size);
		context.GetDeviceContext()->Unmap(m_CBuffer, NULL);
	}

	void DX11UniformBuffer::Bind() const
	{
		DX11Context& context = DX11Context::Get();
		context.GetDeviceContext()->VSSetConstantBuffers(m_Binding, 1, &m_CBuffer);
		//context.GetDeviceContext()->PSSetConstantBuffers(m_Binding, 1, &m_CBuffer);
	}

	void DX11UniformBuffer::Unbind() const
	{
	}

}