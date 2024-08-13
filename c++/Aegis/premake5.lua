workspace "Aegis"
    configurations { "Debug", "Release" }
    platforms { "Win32", "x64", "Mac" }

    filter "platforms:Win32"
        architecture "x32"

    filter "platforms:x64"
        architecture "x64"

    filter "platforms:Mac"
        architecture "x64"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    project "Aegis"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"

        defines { "GLEW_STATIC" }

        files { "Aegis/*.cpp", "Aegis/*.h" }

        vpaths {
            ["Source Files"] = { "Aegis/**.cpp" },
            ["Header Files"] = { "Aegis/**.h" }
        }

        filter "system:windows"
            links { "glfw3", "opengl32", "glew32s", "glu32" }
            libdirs { _MAIN_SCRIPT_DIR .. "/lib/glew/lib", _MAIN_SCRIPT_DIR .. "/lib/glfw/lib", _MAIN_SCRIPT_DIR .. "/lib/SFML/lib" }
            includedirs { "lib/glew/include", "lib/glfw/include", "lib/glm", "lib/SFML/include" }

        filter "system:macosx"
            links { "glfw", "OpenGL.framework", "libGLEW.2.2.0.dylib" }
            libdirs { "/opt/homebrew/Cellar/glfw/3.4/lib", "/opt/homebrew/Cellar/glew/2.2.0_1/lib", "/opt/homebrew/Cellar/sfml/2.6.1/lib" }
            includedirs { "lib/glfw/include", "lib/glew/include", "lib/SFML/include", "lib/glm" }

        debugargs { "+map", "zombieattack.bsp" }
        debugdir (_MAIN_SCRIPT_DIR .. "./Aegis/")

        filter { "configurations:Debug", "system:windows" }
            links { "sfml-system-d", "sfml-audio-d" }

        filter { "configurations:Release", "system:windows" }
            links { "sfml-system", "sfml-audio" }

        filter { "configurations:Debug", "system:macosx" }
            links { "sfml-system", "sfml-audio", "sfml-graphics", "sfml-network", "sfml-window" }

        filter { "configurations:Release", "system:macosx" }
            links { "sfml-system", "sfml-audio", "sfml-graphics", "sfml-network", "sfml-window" }