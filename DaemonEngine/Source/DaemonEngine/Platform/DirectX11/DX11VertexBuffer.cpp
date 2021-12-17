#include "kepch.h"
#include "DX11VertexBuffer.h"
#include "DX11Context.h"

namespace Daemon
{

	DX11VertexBuffer::DX11VertexBuffer(uint32_t size)
		: m_Size(size)
	{
		DX11Context& context = DX11Context::Get();

		ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.ByteWidth = m_Size;
		context.GetDevice()->CreateBuffer(&m_BufferDesc, NULL, &m_BufferHandle);
	}

	DX11VertexBuffer::DX11VertexBuffer(void* vertices, uint32_t size)
		: m_Size(size)
	{
		DX11Context& context = DX11Context::Get();

		ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.ByteWidth = m_Size;

		D3D11_SUBRESOURCE_DATA sr_data = { 0 };
		sr_data.pSysMem = vertices;
		HRESULT hr = context.GetDevice()->CreateBuffer(&m_BufferDesc, &sr_data, &m_BufferHandle);
		KE_CORE_ASSERT(!FAILED(hr) && m_BufferHandle, "Failed to create Vertex Buffer");
	}

	DX11VertexBuffer::~DX11VertexBuffer()
	{
		ReleaseCOM(m_BufferHandle);
	}

	void DX11VertexBuffer::SetData(void* data, uint32_t size)
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		memset(&msr, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

		DX11Context& context = DX11Context::Get();
		context.GetDeviceContext()->Map(m_BufferHandle, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msr);
		memcpy(msr.pData, data, size);
		context.GetDeviceContext()->Unmap(m_BufferHandle, NULL);
	}

	void DX11VertexBuffer::Bind() const
	{
		//uint32_t offset = 0;
		//uint32_t stride = m_BufferLayout.GetStride();
		uint32_t offset = 0;
		uint32_t stride = (3 + 4 + 2 + 1 + 1) * sizeof(float);

		DX11Context& context = DX11Context::Get();
		context.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context.GetDeviceContext()->IASetVertexBuffers(0, 1, &m_BufferHandle, &stride, &offset);
	}

	void DX11VertexBuffer::Unbind() const
	{
	}

}

