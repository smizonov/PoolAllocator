find_package(GTest REQUIRED)

set(This AllocatorTests)

set(Headers
    include/Utils.h)

set(Sources
    AllocatorUnit.cpp
    AllocatorStress.cpp
    Utils.cpp)

add_executable(${This} ${Sources} ${Headers})

target_include_directories(${This} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${This}  PUBLIC GTest::GTest GTest::Main Allocator)

add_test(
    NAME ${This}
    COMMAND ${This}
)
