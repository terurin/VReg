# library
add_library(vreg src/vreg.cpp)
target_include_directories(vreg PUBLIC inc)

# test
add_executable(vreg_test test/vreg_test.cpp test/vregex_test.cpp)

target_link_libraries(vreg_test PRIVATE vreg GTest::gtest_main)
target_include_directories(vreg_test PRIVATE ${GTest}/include inc)
gtest_discover_tests(vreg_test)
