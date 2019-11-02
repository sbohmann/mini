project (mini)
add_executable(mini mini.c errors.c tokenizer.c allocate.c string.c string.h)
set(CMAKE_C_FLAGS "-std=c11")
