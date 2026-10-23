# Test executable – entry point is main (e.g. test/main.cpp)
add_executable(unit_tests
    main.cpp
    # Add more test source files here if needed
)

# Link against the project library + Google Test
target_link_libraries(unit_tests
    PRIVATE
        myMatchingEngineLib
        GTest::gtest   # use GTest::gtest_main if you don't supply your own main
)

# Discover and register tests with CTest
gtest_discover_tests(unit_tests)
