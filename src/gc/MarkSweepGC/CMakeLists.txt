set(MarkSweepGC_SRCS
    MarkSweepGC.h
    MarkSweepGC.cpp
)

add_library(MarkSweepGC STATIC
    ${MarkSweepGC_SRCS}
)

target_include_directories(MarkSweepGC PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)