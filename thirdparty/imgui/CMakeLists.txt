add_library(imgui STATIC
    imgui/imgui_draw.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl2.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/cpp)

if (APPLE)
    find_package(GLFW3 REQUIRED)
    
    target_link_libraries(imgui PRIVATE
        glfw
        "-framework OpenGL"
    )
elseif (UNIX)
    find_package(OpenGL REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)

    include_directories(${GLFW3_INCLUDE_DIRS})
    link_directories(${GLFW3_LIBRARY_DIRS})
    add_definitions(${GLFW3_CFLAGS_OTHER})

    target_link_libraries(imgui PRIVATE
        OpenGL::GL
        ${GLFW3_LIBRARIES}
    )
endif()
