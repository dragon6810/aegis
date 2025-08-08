add_executable(tests
    mathlib/mathlib.cpp
    mathlib/polylib.cpp
    mathlib/collisionlib.cpp
    utilslib/utilslib.cpp
)

target_link_libraries(tests PRIVATE
    gtest
    gtest_main
    mathlib
    utilslib
)

include(GoogleTest)
gtest_discover_tests(tests)