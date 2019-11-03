project (mini)
add_executable(mini mini.c errors.c tokenizer.c allocate.c string.c string.h source.c source.h files.c files.h types.c types.h)
set(CMAKE_C_FLAGS "-std=c11 -O3")
