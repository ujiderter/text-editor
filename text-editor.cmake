cmake_minimum_required(VERSION 3.10)
project(TextEditor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(text_editor text_editor.cpp)

if(MSVC)
    target_compile_options(text_editor PRIVATE /W4)
else()
    target_compile_options(text_editor PRIVATE -Wall -Wextra -pedantic)
endif()

install(TARGETS text_editor
        RUNTIME DESTINATION bin)