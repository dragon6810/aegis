add_executable ( 
    tpkgen ${SOURCES} 

    "src/main.cpp" 
    "src/TpkScript.cpp"
)

if (APPLE)
    if (DEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif()
endif()


target_link_libraries(tpkgen PRIVATE
    scriptlib
    tpklib
)