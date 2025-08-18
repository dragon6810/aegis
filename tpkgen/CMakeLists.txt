add_executable ( 
    tpkgen ${SOURCES} 

    "src/main.cpp" 
    "src/TpkScript.cpp"
)

#if (APPLE)
    
#endif()


target_link_libraries(tpkgen PRIVATE
    scriptlib
    tpklib
)