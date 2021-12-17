#include "kepch.h"
#include "Input.h"

#include "DaemonEngine/Renderer/RendererAPI.h"
#include "DaemonEngine/Platform/GLFW/GLFWInput.h"
#include "DaemonEngine/Platform/Win32/Win32Input.h"

namespace Daemon
{

    Unique<Input> Input::Create()
    {
        switch (RendererAPI::Current())
        {
            case RendererAPIType::OpenGL:       return CreateUnique<Input>(new GLFWInput());
            case RendererAPIType::DirectX11: 
            case RendererAPIType::DirectX12:    return CreateUnique<Input>(new Win32Input());
            case RendererAPIType::Vulkan:
            case RendererAPIType::None:
            default: break;
        }
        KE_CORE_ASSERT("RendererAPIType::None is unsupported!");
        return nullptr;
    }

    Input::Input(InputImpl* inputImpl)
    {
        s_InputImplementationInstance = inputImpl;
    }

    Input::~Input()
    {
        delete s_InputImplementationInstance;
    }

    bool Input::IsKeyPressed(KeyCode key)
    {
        return s_InputImplementationInstance->IsKeyPressed(key);
    }

    bool Input::IsMouseButtonPressed(MouseCode button)
    {
        return s_InputImplementationInstance->IsMouseButtonPressed(button);
    }

    glm::vec2 Input::GetMousePosition()
    {
        return s_InputImplementationInstance->GetMousePosition();
    }

    float Input::GetMouseX()
    {
        return s_InputImplementationInstance->GetMouseX();
    }

    float Input::GetMouseY()
    {
        return s_InputImplementationInstance->GetMouseY();
    }

}