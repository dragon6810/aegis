add_library(bsplib STATIC
    src/BspFile.cpp
)

target_include_directories(bsplib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(bsplib PUBLIC
    eigen
)

target_link_libraries(bsplib PRIVATE
    parselib
)

target_include_directories(bsplib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)