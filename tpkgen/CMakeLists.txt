add_executable ( 
    tpkgen ${SOURCES} 

    "src/main.cpp" 
)

if (APPLE)
    if (DEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif()
endif()


target_link_libraries(tpkgen PRIVATE
    parselib
    tpklib
)