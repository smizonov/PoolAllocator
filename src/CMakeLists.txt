set(Headers
    ../include/Allocator.h)

set(Sources
    Allocator.cpp)

add_library(Allocator
    ${Sources} ${Headers}
)

target_include_directories(Allocator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
