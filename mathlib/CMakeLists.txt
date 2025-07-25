add_library(mathlib STATIC
    src/mathlib.cpp
)

target_link_libraries(mathlib PRIVATE
    eigen
)

target_include_directories(mathlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)