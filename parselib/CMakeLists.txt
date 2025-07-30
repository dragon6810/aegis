add_library(parselib STATIC
    src/Tokenizer
)

target_link_libraries(parselib PUBLIC
    mathlib
)

target_include_directories(parselib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(parselib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)