add_library(engine STATIC
    "src/cl_Client.cpp"
    "src/sv_Server.cpp"
    "src/Console.cpp"
    "src/PlayerMove.cpp"
    "src/cl_PlayerInput.cpp"
    "src/cl_Input.cpp"
)

target_link_libraries(engine PRIVATE
    utilslib
)

target_link_libraries(engine PUBLIC
    SDL3::SDL3
)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)