# fuck cmake. I'd be using makefiles if it werent for the visual studio integration.
# cmake is a fragile piece of shit that never works.
# fuck cmake.

# TO COMPILE ON MAC:
# brew install glew glfw sfml

# TO COMPILE ON DEBIAN:
# sudo apt update
# sudo apt install libglew-dev libglfw3-dev libsfml-dev libglu1-mesa-dev

cmake_minimum_required(VERSION 3.10)
project(Aegis)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Include directories for all the libraries
include_directories(
    external/glew/include
    external/glm
    external/SFML/include
    src
    clib
)

add_executable ( 
    Aegis ${SOURCES} 
    "src/main.cpp" 
    "src/Game.cpp" 
    "src/World.cpp" 
    "src/Window.cpp" 
    "src/Renderer.cpp" 
    "src/Command.cpp" 
    "src/ResourceManager.cpp" 
    "src/Input.cpp" 
    "src/Wad.cpp" 
    "src/Console.cpp" 
    "src/RasterFont.cpp" 
    "src/Vector.cpp" 
    "src/EntityBase.cpp" 
    "src/EntityCamera.cpp" 
    "src/Quaternion.cpp" 
    "src/Matrix.cpp" 
    "src/EntityStudio.cpp" 
    "src/EntityPlayer.cpp" 
    "src/NavMesh.cpp" 
    "src/PolyMath.cpp" 
    "src/NavAgent.cpp" 
    "src/WalkAgent.cpp" 
    "src/EntityZombie.cpp"
    "src/Camera.cpp"
)

set_target_properties(Aegis PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(run-cppcheck
        COMMAND ${CPPCHECK} --enable=all --inconclusive --quiet ${CMAKE_SOURCE_DIR}
        COMMENT "Running static analysis with Cppcheck"
        VERBATIM)
endif()

if (APPLE)
    if (DEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif()

    find_package(GLEW REQUIRED)
    find_package(GLFW3 REQUIRED)
    find_package(SFML COMPONENTS System Audio REQUIRED)
    
    target_link_libraries(Aegis PRIVATE
        GLEW::GLEW
        glfw
        SFML::Audio
        SFML::System
        "-framework OpenGL"
    )
elseif (WIN32)
    target_link_libraries(Aegis PRIVATE
        ${CMAKE_SOURCE_DIR}/external/glew/lib/glew32.lib
        ${CMAKE_SOURCE_DIR}/external/glfw/lib/glfw3.lib
        ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-system.lib
        ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-window.lib
        ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-graphics.lib
        ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-audio.lib
        opengl32
    )

    add_custom_command(TARGET Aegis POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/external/glew/bin/glew32.dll" $<TARGET_FILE_DIR:Aegis>)
elseif (UNIX)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(SFML COMPONENTS system audio REQUIRED)

    # Use pkg-config to find GLFW
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)

    include_directories(${GLFW3_INCLUDE_DIRS})
    link_directories(${GLFW3_LIBRARY_DIRS})
    add_definitions(${GLFW3_CFLAGS_OTHER})

    target_link_libraries(Aegis PRIVATE
        OpenGL::GL
        GLEW::GLEW
        ${GLFW3_LIBRARIES}
        sfml-system
        sfml-audio
    )
endif()
