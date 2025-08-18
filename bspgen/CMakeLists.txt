add_executable(bspgen
    src/main.cpp
    src/HullDef.cpp
    src/Bsp.cpp
    src/Csg.cpp
    src/Portals.cpp
    src/Write.cpp
)

target_include_directories(bspgen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(bspgen PRIVATE
    eigen
    argparse
    utilslib
    maplib
    tpklib
    bsplib
)