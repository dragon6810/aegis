add_executable(tests
    mathlib/mathlib.cpp
)

target_link_libraries(tests PRIVATE
    gtest
    gtest_main
    mathlib
)

include(GoogleTest)
gtest_discover_tests(tests)