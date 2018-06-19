set(Value_SRCS
    Value.h
    Value.cpp
)

add_library(Value STATIC
    ${Value_SRCS}
)

target_include_directories(Value PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)