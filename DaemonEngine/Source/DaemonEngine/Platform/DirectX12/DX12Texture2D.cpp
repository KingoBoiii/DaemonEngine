#include "kepch.h"
#include "DX12Texture2D.h"
#include "DX12Context.h"

#include "stb_image.h"

namespace Daemon
{

	DX12Texture2D::DX12Texture2D(const std::string& filepath, const TextureSpecification& specification)
		: m_Width(0), m_Height(0), m_Specification(specification)
	{
        CreateCommandList();
        CreateTexture();
    }

	DX12Texture2D::DX12Texture2D(uint32_t width, uint32_t height, const TextureSpecification& specification)
		: m_Width(width), m_Height(height), m_Specification(specification)
	{
        CreateCommandList();
        CreateTexture();
    }

	DX12Texture2D::DX12Texture2D(uint32_t width, uint32_t height, void* data, const TextureSpecification& specification)
		: m_Width(width), m_Height(height), m_Specification(specification)
	{
        CreateCommandList();
        CreateTexture();
    }

	DX12Texture2D::~DX12Texture2D()
	{
	}

	void DX12Texture2D::SetData(void* data) const
	{
	}

	void DX12Texture2D::Bind(uint32_t slot) const
	{
	}

	void DX12Texture2D::Unbind() const
	{
	}

	void DX12Texture2D::CreateTexture()
	{
        CreateSRVHeap();

        DX12Context context = DX12Context::Get();
        ID3D12Device8* device = context.GetDevice();

        ID3D12Resource* textureUploadHeap;

        // Describe and create a Texture2D.
        D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.MipLevels = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.Width = 1;
        textureDesc.Height = 1;
        textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        textureDesc.DepthOrArraySize = 1;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_Texture));
        KE_CORE_ASSERT(!FAILED(hr), "Failed to create Texture");

        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Texture, 0, 1);

        // Create the GPU upload buffer.
        hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&textureUploadHeap));
        KE_CORE_ASSERT(!FAILED(hr), "Failed to create GPU upload buffer");

        // Copy data to the intermediate upload heap and then schedule a copy 
        // from the upload heap to the Texture2D.
        //std::vector<UINT8> texture = GenerateTextureData();
        int32_t width, height, channels;
        const stbi_uc* data = stbi_load("Assets/Textures/cartoon-rocket.png", &width, &height, &channels, 0);

        D3D12_SUBRESOURCE_DATA textureData = {};
        textureData.pData = &data[0];
        textureData.RowPitch = static_cast<uint64_t>(m_Width * channels);
        textureData.SlicePitch = textureData.RowPitch * m_Height;

        m_CommandAllocator->Reset();
        m_CommandList->Reset(m_CommandAllocator, nullptr);

        UpdateSubresources(m_CommandList, m_Texture, textureUploadHeap, 0, 0, 1, &textureData);
        m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

        m_CommandList->Close();

        // Describe and create a SRV for the texture.
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        device->CreateShaderResourceView(m_Texture, &srvDesc, m_SRVHeap->GetCPUDescriptorHandleForHeapStart());
    }

    void DX12Texture2D::CreateSRVHeap()
    {
        ID3D12Device8* device = DX12Context::Get().GetDevice();

        // Describe and create a shader resource view (SRV) heap for the texture.
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = 1;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        HRESULT hr = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_SRVHeap));
        KE_CORE_ASSERT(!FAILED(hr), "Failed to create SRV heap");
    }

    void DX12Texture2D::CreateCommandList()
    {
        ID3D12Device8* device = DX12Context::Get().GetDevice();

        HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
        KE_CORE_ASSERT(!FAILED(hr), "Failed to create Command Allocator");

        hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator, nullptr, IID_PPV_ARGS(&m_CommandList));
        KE_CORE_ASSERT(!FAILED(hr), "Failed to create Command List");

        // When the CommandList is created, it starts in "Recording" state, so close it
        m_CommandList->Close();
    }

}