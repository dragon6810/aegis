add_library(engine STATIC
    "src/cl_Client.cpp"
    "src/sv_Server.cpp"
    "src/Console.cpp"
    "src/cl_PlayerInput.cpp"
    "src/cl_Input.cpp"
    "src/Player.cpp"
    "src/sv_NetClient.cpp"
    "src/NetChan.cpp"
    "src/cl_DumbClient.cpp"
)

target_link_libraries(engine PRIVATE
    utilslib
    mathlib
)

target_link_libraries(engine PUBLIC
    eigen
    SDL3::SDL3
    renderer
)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)