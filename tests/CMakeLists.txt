add_executable(tests
    mathlib/mathlib.cpp
    mathlib/polylib.cpp
)

target_link_libraries(tests PRIVATE
    gtest
    gtest_main
    mathlib
)

include(GoogleTest)
gtest_discover_tests(tests)