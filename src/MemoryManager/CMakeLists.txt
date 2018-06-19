set(MemoryManager_SRCS
    MemoryManager.h
    MemoryManager.cpp
)

add_library(MemoryManager STATIC
    ${MemoryManager_SRCS}
)

target_include_directories(MemoryManager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)