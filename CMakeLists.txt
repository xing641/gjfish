set(CMAKE_C_COMPILER "/data/home/zxyang/zx_path/bin/gcc")
set(CMAKE_CXX_COMPILER "/data/home/zxyang/zx_path/bin/g++")
cmake_minimum_required(VERSION 3.10)
project(src_stable C CXX)

set(CMAKE_CXX_STANDARD 17)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# set(CMAKE_CXX_FLAGS "-Wall -Wextra")
# set(CMAKE_CXX_FLAGS_DEBUG "-g")
# set(CMAKE_CXX_FLAGS_RELEASE "-O2")

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
        src/gfa_reader.cc
        src/kmer_coder.h
        src/kmer_coder.cc
        src/kmer_counter.h
        src/kmer_counter.cc
        src/mem_allocator.cc
        src/mem_allocator.h
        src/lockfree_hash_map.cc
        src/lockfree_hash_map.h
        src/buffer_queue.h
        src/buffer_queue.cc
        src/test.cc
        # src/graph_gen.cpp
        src/param.cc
        src/param.h)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(src_stable PUBLIC Threads::Threads)
