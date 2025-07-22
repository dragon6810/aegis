add_library(imgui STATIC
    src/imgui_draw.cpp
    src/imgui_tables.cpp
    src/imgui_widgets.cpp
    src/imgui.cpp
    src/backends/imgui_impl_glfw.cpp
    src/backends/imgui_impl_opengl2.cpp
)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public/backends)

if (APPLE)
    if (DEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif()

    #find_package(GLEW REQUIRED)
    find_package(GLFW3 REQUIRED)
    
    target_link_libraries(imgui PRIVATE
        #glm
        #glew 
        #GLEW::GLEW
        glfw
        "-framework OpenGL"
    )
elseif (UNIX)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)

    include_directories(${GLFW3_INCLUDE_DIRS})
    link_directories(${GLFW3_LIBRARY_DIRS})
    add_definitions(${GLFW3_CFLAGS_OTHER})

    target_link_libraries(imgui PRIVATE
        glm
        #glew
        imgui
        OpenGL::GL
        #GLEW::GLEW
        ${GLFW3_LIBRARIES}
    )
endif()