add_library(utilslib STATIC
    src/utilslib.cpp
)

target_include_directories(utilslib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(utilslib PRIVATE
    cpptrace-lib
    mathlib
)

target_include_directories(utilslib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)