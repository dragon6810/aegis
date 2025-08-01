add_library(cfglib STATIC
    src/CfgFile.cpp
)

target_include_directories(cfglib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(cfglib PUBLIC
    parselib
)

target_include_directories(cfglib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)