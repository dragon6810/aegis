# fuck cmake. I'd be using makefiles if it werent for the visual studio integration.
# cmake is a fragile piece of shit that never works.
# fuck cmake.

# TO COMPILE ON MAC:
# brew install glew glfw sfml

# TO COMPILE ON DEBIAN:
# sudo apt update
# sudo apt install libglew-dev libglfw3-dev libsfml-dev libglu1-mesa-dev

cmake_minimum_required(VERSION 3.10)
project(WP3D)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Include directories for all the libraries
include_directories(
    external/glew/include
    external/glm
    external/SFML/include
    external/imgui
    external/imgui/backends
    src
)

add_executable ( 
    WP3D ${SOURCES} 
    
    "external/imgui/imgui.cpp" 
    "external/imgui/imgui_draw.cpp" 
    "external/imgui/imgui_tables.cpp" 
    "external/imgui/imgui_widgets.cpp" 
    "external/imgui/backends/imgui_impl_glfw.cpp" 
    "external/imgui/backends/imgui_impl_opengl2.cpp" 

    "src/main.cpp" 
    "src/Gui.cpp"
)

set_target_properties(WP3D PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

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
    
    target_link_libraries(WP3D PRIVATE
        GLEW::GLEW
        glfw
        SFML::Audio
        SFML::System
        "-framework OpenGL"
    )
elseif (WIN32)
    target_link_libraries(WP3D PRIVATE
        ${CMAKE_SOURCE_DIR}/external/glew/lib/glew32.lib
        ${CMAKE_SOURCE_DIR}/external/glfw/lib/glfw3.lib
        ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-system.lib
        ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-window.lib
        ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-graphics.lib
        ${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-audio.lib
        opengl32
    )

    add_custom_command(TARGET WP3D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/external/glew/bin/glew32.dll" $<TARGET_FILE_DIR:WP3D>)
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

    target_link_libraries(WP3D PRIVATE
        OpenGL::GL
        GLEW::GLEW
        ${GLFW3_LIBRARIES}
        sfml-system
        sfml-audio
    )
endif()
