#include "kepch.h"
#include "DX11IndexBuffer.h"

#include "DX11Context.h"

namespace Daemon
{

	DX11IndexBuffer::DX11IndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		DX11Context& context = DX11Context::Get();

		D3D11_BUFFER_DESC indexBufferdesc = {};
		indexBufferdesc.ByteWidth = count * sizeof(uint32_t);
		indexBufferdesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA sr_data = { 0 };
		sr_data.pSysMem = indices;
		context.GetDevice()->CreateBuffer(&indexBufferdesc, &sr_data, &m_BufferHandle);
		KE_CORE_ASSERT(m_BufferHandle, "Failed to create Index buffer!");
	}

	DX11IndexBuffer::~DX11IndexBuffer()
	{
		ReleaseCOM(m_BufferHandle);
	}

	void DX11IndexBuffer::Bind() const
	{
		DX11Context& context = DX11Context::Get();
		context.GetDeviceContext()->IASetIndexBuffer(m_BufferHandle, DXGI_FORMAT_R32_UINT, 0);
	}

	void DX11IndexBuffer::Unbind() const
	{
	}

}