set(MarkCompactGC_SRCS
    MarkCompactGC.h
    MarkCompactGC.cpp
)

add_library(MarkCompactGC STATIC
    ${MarkCompactGC_SRCS}
)

target_include_directories(MarkCompactGC PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)