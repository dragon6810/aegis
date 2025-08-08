add_library(scriptlib STATIC
    src/ScriptFile.cpp
    src/ScriptStatement.cpp
)

target_include_directories(scriptlib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(scriptlib PUBLIC
    parselib
)

target_include_directories(scriptlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)