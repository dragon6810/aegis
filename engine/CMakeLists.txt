add_executable ( 
    engine ${SOURCES} 
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
    "src/EntityBase.cpp" 
    "src/EntityCamera.cpp" 
    "src/EntityStudio.cpp" 
    "src/EntityPlayer.cpp" 
    "src/NavMesh.cpp" 
    "src/PolyMath.cpp" 
    "src/NavAgent.cpp" 
    "src/WalkAgent.cpp" 
    "src/EntityZombie.cpp"
    "src/Camera.cpp"
)

if (APPLE)
    if (DEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif()

    find_package(GLEW REQUIRED)
    find_package(GLFW3 REQUIRED)
    
    target_link_libraries(engine PRIVATE
        glm
        glew 
        SFML::Audio
        eigen
        mathlib
        GLEW::GLEW
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

    target_link_libraries(engine PRIVATE
        glm
        glew
        SFML::Audio
        eigen
        mathlib
        OpenGL::GL
        GLEW::GLEW
        ${GLFW3_LIBRARIES}
    )
endif()