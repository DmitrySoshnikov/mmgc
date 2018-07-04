cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 17)

project(mmgc)

enable_testing()

add_subdirectory(src)
add_subdirectory(src/Value)
add_subdirectory(src/allocators/SingleFreeListAllocator)
add_subdirectory(src/MemoryManager)
add_subdirectory(src/gc/MarkSweepGC)
add_subdirectory(test)
