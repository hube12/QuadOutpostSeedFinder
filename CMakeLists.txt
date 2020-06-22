cmake_minimum_required(VERSION 3.10)
project(findQuadOutpost LANGUAGES C)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g -O2 -static-libgcc -static")
find_package(Threads REQUIRED)
add_executable(findQuadOutpost layers.c generator.c finders.c findQuadOutpost.c)

target_link_libraries(findQuadOutpost m)
target_link_libraries(findQuadOutpost Threads::Threads)