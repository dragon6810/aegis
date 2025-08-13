add_executable(launcher ${SOURCES}
    "src/main.cpp"
)

target_link_libraries(launcher PRIVATE
    argparse
    engine
    game
)