add_executable(bspgen
    src/main.cpp
    src/BspBuilder.cpp
    src/Entity.cpp
    src/Brush.cpp
    src/BrFace.cpp
)

target_include_directories(bspgen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(bspgen PRIVATE
    eigen
    argparse
    utilslib
    maplib
    tpklib
)