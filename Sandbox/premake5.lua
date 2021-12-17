project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"Source/**.cpp",
		"Source/**.h",
	}
	
	includedirs {
		"%{wks.location}/DaemonEngine/Source",
		"%{wks.location}/DaemonEngine/Vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.freetype}",
		"%{IncludeDir.box2d}",
		"%{IncludeDir.entt}",
	}

	links {
		"DaemonEngine"
	}

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