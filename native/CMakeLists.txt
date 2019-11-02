project (hello)
add_executable(mini mini.c errors.c tokenizer.c allocate.c)
set(CMAKE_C_FLAGS "-D_CRT_SECURE_NO_DEPRECATE")
