cmake_minimum_required(VERSION 3.10)
project(Aegis)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# third-party
add_subdirectory(thirdparty/glew)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/SFML)
add_subdirectory(thirdparty/eigen)
add_subdirectory(thirdparty/imgui)

# libraries
add_subdirectory(mathlib)

# results
add_subdirectory(engine)
add_subdirectory(wp3d)