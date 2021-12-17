project "DaemonEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "kepch.h"
	pchsource "Source/kepch.cpp"

	files {
		"Source/**.cpp",
		"Source/**.h",
		"Vendor/stb_image/**.h",
		"Vendor/stb_image/**.cpp",
		"Vendor/glm/glm/**.hpp",
		"Vendor/glm/glm/**.inl",

		"Vendor/ImGuizmo/ImGuizmo.h",
		"Vendor/ImGuizmo/ImGuizmo.cpp",
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs {
		"Source",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.stb_image}",	
		"%{IncludeDir.freetype}",	
		"%{IncludeDir.ftgl}",	
		"%{IncludeDir.box2d}",	
		"%{IncludeDir.entt}",	
		"%{IncludeDir.yaml_cpp}",	
		"%{IncludeDir.imguizmo}",	
	}

	links {
		"GLFW",
		"GLAD",
		"imgui",
		"FreeType",
		"Box2D",
		"yaml-cpp",
		"opengl32.dll",
	}

	filter "files:Vendor/ImGuizmo/**.cpp"
		flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"
		-- defines "KE_PLATFORM_WINDOWS"

	filter "configurations:Debug"
		defines "KE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "KE_RELEASE"
		runtime "Release"
		optimize "on"
