add_executable ( 
    wp3d ${SOURCES} 

    "src/main.cpp" 
    "src/Gui.cpp"
    "src/Map.cpp"
    "src/Brush.cpp"
    "src/Plane.cpp"
    "src/Entity.cpp"
    "src/Viewport.cpp"
    "src/GuiElement.cpp"
    "src/GuiElementViewport.cpp"
    "src/GuiElementMenuBar.cpp"
    "src/GuiElementConfigMenu.cpp"
    "src/GuiElementToolBar.cpp"
    "src/GuiElementToolSettings.cpp"
    "src/GuiElementEntityEditor.cpp"
    "src/GuiElementTextureBrowser.cpp"
    "src/TextureManager.cpp"
    "src/GuiElementTextureMapping.cpp"
)

if (APPLE)
    if (DEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif()

    target_compile_definitions(wp3d PRIVATE __MACOS)

    find_package(GLEW REQUIRED)
    find_package(GLFW3 REQUIRED)
    
    target_link_libraries(wp3d PRIVATE
        glm
        glew 
        imgui
        ImGuiFileDialog
        eigen
        mathlib
        maplib
        fgdlib
        cfglib
        tpklib
        GLEW::GLEW
        glfw
        "-framework OpenGL"
    )
elseif (UNIX)
    target_compile_definitions(wp3d PRIVATE __LINUX)

    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)

    include_directories(${GLFW3_INCLUDE_DIRS})
    link_directories(${GLFW3_LIBRARY_DIRS})
    add_definitions(${GLFW3_CFLAGS_OTHER})

    target_link_libraries(wp3d PRIVATE
        glm
        glew
        imgui
        ImGuiFileDialog
        eigen
        mathlib
        maplib
        fgdlib
        cfglib
        tpklib
        OpenGL::GL
        GLEW::GLEW
        ${GLFW3_LIBRARIES}
    )
endif()