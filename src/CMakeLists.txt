set(mmgc_SRCS
    main.cpp
)

add_executable(mmgc
    ${mmgc_SRCS}
)

target_link_libraries(mmgc
    Value
    MemoryManager
    SingleFreeListAllocator
    MarkSweepGC
)

install(TARGETS mmgc DESTINATION bin)