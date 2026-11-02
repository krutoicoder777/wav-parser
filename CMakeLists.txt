cmake_minimum_required(VERSION 3.10)
project(wav_parser C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(inc)

add_executable(wav_parser
    src/main.c
    src/wav_parser.c
)

if (UNIX)
    target_link_libraries(wav_parser m)
endif()
