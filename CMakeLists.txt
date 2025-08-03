cmake_minimum_required(VERSION 3.10)
project(Aegis)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Third-party
add_subdirectory(thirdparty/glew)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/SFML)
add_subdirectory(thirdparty/eigen)
add_subdirectory(thirdparty/imgui)
add_subdirectory(thirdparty/filedialog)
add_subdirectory(thirdparty/googletest)
add_subdirectory(thirdparty/zlib)

# Libraries
add_subdirectory(mathlib)
add_subdirectory(parselib)
add_subdirectory(maplib)
add_subdirectory(fgdlib)
add_subdirectory(cfglib)

# Executables
set(EXEC_TARGET "ALL" CACHE STRING "Executable to build (ALL, tests, engine, wp3d)")
if(EXEC_TARGET STREQUAL "ALL" OR EXEC_TARGET STREQUAL "tests")
    add_subdirectory(tests)
endif()

if(EXEC_TARGET STREQUAL "ALL" OR EXEC_TARGET STREQUAL "engine")
    add_subdirectory(engine)
endif()

if(EXEC_TARGET STREQUAL "ALL" OR EXEC_TARGET STREQUAL "wp3d")
    add_subdirectory(wp3d)
endif()