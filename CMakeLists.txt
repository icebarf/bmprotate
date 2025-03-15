cmake_minimum_required(VERSION 3.25)
project(bmprotate VERSION 0.0.1)

set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(bmprotate)
target_sources(bmprotate PUBLIC src/benchmark.c
                         PUBLIC src/bitmap.c
                         PUBLIC src/util.c)
target_compile_options(bmprotate PUBLIC -Wall -Wextra -Wshadow -Wpedantic -Wconversion -g -Og)