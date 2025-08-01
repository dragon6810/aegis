add_library(fgdlib STATIC
    src/FgdFile.cpp
    src/EntityDef.cpp 
    src/EntityPair.cpp
)

target_include_directories(fgdlib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(fgdlib PUBLIC
    parselib
)

target_include_directories(fgdlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)