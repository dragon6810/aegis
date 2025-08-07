add_executable(bspgen
    src/main.cpp
)

target_include_directories(bspgen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(bspgen PRIVATE
    eigen
    argparse
)