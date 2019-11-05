project (mini)
file(GLOB_RECURSE sources "*.c")
add_executable($sources)
set(CMAKE_C_FLAGS "-std=c11 -O3")
