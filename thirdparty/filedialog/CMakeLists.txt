cmake_minimum_required(VERSION 3.20)

project(ImGuiFileDialog)

add_library(ImGuiFileDialog STATIC
    ImGuiFileDialog/ImGuiFileDialog.cpp
    ImGuiFileDialog/ImGuiFileDialog.h
    ImGuiFileDialog/ImGuiFileDialogConfig.h
)

target_include_directories(ImGuiFileDialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ImGuiFileDialog)

if(UNIX)
    target_compile_options(ImGuiFileDialog PUBLIC -Wno-unknown-pragmas)
endif()

target_link_libraries(ImGuiFileDialog PUBLIC
    imgui
)
