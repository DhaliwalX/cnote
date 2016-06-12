cmake_minimum_required (VERSION 2.6)
project (cnote)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Werror")

# search for boost library and program_options is required
find_package (Boost COMPONENTS program_options REQUIRED)

set (Boost_USE_MULTITHREAD ON)

add_executable (cnote cnote.cpp cnote-impl.cpp markdown.cpp)
target_link_libraries (cnote ${Boost_LIBRARIES})

