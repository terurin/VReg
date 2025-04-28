# demo
add_executable(vreg_demo src/main.cpp)
target_include_directories(vreg PRIVATE inc vreg)
target_link_libraries(vreg_demo PRIVATE vreg)
