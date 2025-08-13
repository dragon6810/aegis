cmake_minimum_required(VERSION 3.10)
project(Aegis)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if (APPLE)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif()
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

# Third-party
add_subdirectory(thirdparty/glew)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/eigen)
add_subdirectory(thirdparty/imgui)
add_subdirectory(thirdparty/filedialog)
add_subdirectory(thirdparty/googletest)
add_subdirectory(thirdparty/cpptrace)
add_subdirectory(thirdparty/zlib)
add_subdirectory(thirdparty/argparse)

# Libraries
add_subdirectory(mathlib)
add_subdirectory(utilslib)
add_subdirectory(parselib)
add_subdirectory(scriptlib)
add_subdirectory(maplib)
add_subdirectory(fgdlib)
add_subdirectory(cfglib)
add_subdirectory(tpklib)
add_subdirectory(bsplib)
add_subdirectory(engine)
add_subdirectory(game)

# Executables
set(EXEC_TARGET "ALL" CACHE STRING "Executable to build (ALL, tests, launcher, wp3d, tpkgen, bspgen)")
if(EXEC_TARGET STREQUAL "ALL" OR EXEC_TARGET STREQUAL "tests")
    add_subdirectory(tests)
endif()

if(EXEC_TARGET STREQUAL "ALL" OR EXEC_TARGET STREQUAL "launcher")
    add_subdirectory(launcher)
endif()

if(EXEC_TARGET STREQUAL "ALL" OR EXEC_TARGET STREQUAL "wp3d")
    add_subdirectory(wp3d)
endif()

if(EXEC_TARGET STREQUAL "ALL" OR EXEC_TARGET STREQUAL "tpkgen")
    add_subdirectory(tpkgen)
endif()

if(EXEC_TARGET STREQUAL "ALL" OR EXEC_TARGET STREQUAL "bspgen")
    add_subdirectory(bspgen)
endif()