add_library(maplib STATIC
    src/MapFile.cpp
)

target_include_directories(maplib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(maplib PUBLIC
    eigen
    parselib
)

target_include_directories(maplib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)