#include "kepch.h"
#include "Shader.h"

#include "RendererAPI.h"
#include "DaemonEngine/Platform/OpenGL/OpenGLShader.h"
#include "DaemonEngine/Platform/DirectX11/DX11Shader.h"
#include "DaemonEngine/Platform/DirectX12/DX12Shader.h"

namespace Daemon
{

	Shared<Shader> Shader::Create(const std::string& filepath, const ShaderSpecification& specification)
	{
		switch (RendererAPI::Current())
		{
            case RendererAPIType::OpenGL:       return CreateShared<OpenGLShader>(filepath);
            case RendererAPIType::DirectX11:    return CreateShared<DX11Shader>(filepath, specification);
            case RendererAPIType::DirectX12:    return CreateShared<DX12Shader>(filepath, specification);
            case RendererAPIType::Vulkan:
            case RendererAPIType::None:
			default: break;
		}
        KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
		return nullptr;
	}

    Shared<Shader> Shader::Create(const std::string& name, const std::string& filepath, const ShaderSpecification& specification)
    {
        switch (RendererAPI::Current())
        {
            case RendererAPIType::OpenGL:       return CreateShared<OpenGLShader>(name, filepath);
            case RendererAPIType::DirectX11:    return CreateShared<DX11Shader>(name, filepath, specification);
            case RendererAPIType::DirectX12:    return CreateShared<DX12Shader>(name, filepath, specification);
            case RendererAPIType::Vulkan:
            case RendererAPIType::None:
            default: break;
        }
        KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
        return nullptr;
    }

    Shared<ShaderLibrary> ShaderLibrary::Create()
    {
        return CreateShared<ShaderLibrary>();
    }

    void ShaderLibrary::Add(const Shared<Shader>& shader)
    {
        auto& name = shader->GetName();
        KE_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Load(const std::string& filepath)
    {
        auto shader = Shader::Create(filepath);
        auto& name = shader->GetName();
        KE_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Load(const std::string& name, const std::string& filepath)
    {
        KE_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
        m_Shaders[name] = Shader::Create(name, filepath);
    }

    const Shared<Shader>& ShaderLibrary::Get(const std::string& name) const
    {
        KE_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end());
        return m_Shaders.at(name);
    }

}