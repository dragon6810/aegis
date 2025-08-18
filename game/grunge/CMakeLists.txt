add_library(game ${SOURCES}
    src/Game.cpp
)

target_link_libraries(game PUBLIC
    engine
)

target_include_directories(game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)