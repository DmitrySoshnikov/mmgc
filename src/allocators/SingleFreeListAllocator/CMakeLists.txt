set(SingleFreeListAllocator_SRCS
    SingleFreeListAllocator.h
    SingleFreeListAllocator.cpp
)

add_library(SingleFreeListAllocator STATIC
    ${SingleFreeListAllocator_SRCS}
)

target_include_directories(SingleFreeListAllocator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)