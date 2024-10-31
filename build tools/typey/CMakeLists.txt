cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Awol C)

# Include the source files from src/ and ../common/
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(COMMON_DIR ${CMAKE_SOURCE_DIR}/../common)

# Add the source files to a variable
set(SOURCES
    ${SRC_DIR}/main.c ${COMMON_DIR}/bspfile.c ${COMMON_DIR}/bspfile.h ${COMMON_DIR}/defs.h ${COMMON_DIR}/gfile.c ${COMMON_DIR}/gfile.h ${COMMON_DIR}/mapdefs.h ${COMMON_DIR}/polygon.c ${COMMON_DIR}/polygon.h ${COMMON_DIR}/textfile.c ${COMMON_DIR}/textfile.h ${COMMON_DIR}/vector.c ${COMMON_DIR}/vector.h ${COMMON_DIR}/bitmap.c ${COMMON_DIR}/bitmap.h ${COMMON_DIR}/wad.c ${COMMON_DIR}/wad.h
)

# Include the common/ directory for headers
include_directories(${COMMON_DIR})

# Add an executable target
add_executable(${PROJECT_NAME} ${SOURCES})
