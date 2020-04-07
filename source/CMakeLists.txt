cmake_minimum_required(VERSION 3.15)
project(MSDscript)

set(CMAKE_CXX_STANDARD 17)

add_executable(MSDscript main.cpp parser.cpp Expr.hpp parser.hpp Expr.cpp value.cpp value.hpp pointer.hpp env.cpp env.hpp Step.cpp Step.hpp Cont.cpp Cont.hpp)

set(CMAKE_CXX_FLAGS "-fprofile-instr-generate -fcoverage-mapping")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")

