add_library(tpklib STATIC
    src/TpkFile.cpp
)

target_include_directories(tpklib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(tpklib PUBLIC
    eigen
    zlib
)

target_include_directories(tpklib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)