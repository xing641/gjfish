cmake_minimum_required(VERSION 3.10)
project(src_stable C CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(src)

#add_executable(src_stable
#        src/hash_counter.cpp
#        src/hash_counter.hpp
#        src/hash_table.cpp
#        src/hash_table.hpp
#        src/kmer_processor.cpp
#        src/kmer_processor.hpp
#        src/mem_allocator.cpp
#        src/mem_allocator.hpp
#        src/utils.cpp
#        src/utils.hpp
#        src/main.cpp
#        src/param.cpp
#        src/param.hpp
#        test.cpp
#        test.hpp)
add_executable(src_stable
        src/gfa_reader.h
        src/gfa_reader.cc)