add_library(engine STATIC
    "src/cl_Client.cpp"
    "src/sv_Server.cpp"
)

target_link_libraries(engine PRIVATE
    utilslib
)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)