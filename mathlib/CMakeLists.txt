add_library(mathlib STATIC
    src/mathlib.cpp
    src/polylib.cpp
)

target_include_directories(mathlib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(mathlib PUBLIC
    eigen
)

target_include_directories(mathlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)