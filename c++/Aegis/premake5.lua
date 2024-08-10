workspace "Aegis"
    configurations { "Debug", "Release" }
    platforms { "Win32", "x64"};

    filter "platforms:Win32"
        architecture  "x32"

    filter "platforms:x64"
        architecture  "x64"

    filter "configurations:Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    project "Aegis"
        kind "ConsoleApp"
        language "C++"

        defines { "GLEW_STATIC" }

        files { "Aegis/*.cpp", "Aegis/*.h" }

        vpaths {
            ["Source Files"] = { "Aegis/**.cpp" },
            ["Header Files"] = { "Aegis/**.h" }
        }

        links { "glfw3", "opengl32", "glew32s", "glu32" }
        libdirs {_MAIN_SCRIPT_DIR .. "/lib/glew/lib", _MAIN_SCRIPT_DIR .. "/lib/glfw/lib", _MAIN_SCRIPT_DIR .. "/lib/SFML/lib"}
        includedirs {"lib/glew/include", "lib/glfw/include", "lib/glm", "lib/SFML/include"}

        debugargs { "+map", "zombieattack.bsp" }
        debugdir (_MAIN_SCRIPT_DIR .. "./Aegis/")

        filter "configurations:Debug"
            links { "sfml-system-d", "sfml-audio-d" }

        filter "configurations:Release"
            links { "sfml-system", "sfml-audio" }